#include <simulation/SignalSolver.hpp>
#include <core/Logger.hpp>
#include <utils/DataExporter.hpp>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>

using namespace math;

namespace simulation
{
    SignalSolver::SignalSolver(std::shared_ptr<SimulationScene> scene)
        : scene_(std::move(scene))
    {
    }

    std::shared_ptr<math::PointCloud> SignalSolver::solve()
    {
        // First, calculate ToF points and build the matrix
        auto result = std::make_shared<PointCloud>();
        auto allPoints = scene_->getMergedPointCloud();

        if (!allPoints || allPoints->empty())
        {
            return result;
        }

        const auto &transmitters = scene_->getTransmitters();
        const auto &receivers = scene_->getReceivers();

        if (transmitters.empty() || receivers.empty())
        {
            return result;
        }

        TofMatrix tofMatrix(transmitters.size(), receivers.size());

        // Calculate ToF values and collect points
        for (size_t txIndex = 0; txIndex < transmitters.size(); ++txIndex)
        {
            const auto &transmitter = transmitters[txIndex];

            for (size_t rxIndex = 0; rxIndex < receivers.size(); ++rxIndex)
            {
                const auto &receiver = receivers[rxIndex];

                auto filteredPoints = filterPointsByFov(allPoints, transmitter, receiver);
                if (!filteredPoints || filteredPoints->empty())
                {
                    continue;
                }

                auto closestPoint = findClosestPointInFov(filteredPoints, transmitter, receiver);
                float totalDistance =
                    closestPoint.distanceTo(transmitter->getTransformNode()->getGlobalTransform().getPosition()) +
                    closestPoint.distanceTo(receiver->getTransformNode()->getGlobalTransform().getPosition());

                tofMatrix(txIndex, rxIndex) = totalDistance;
                solveCount_++;
                result->addPoint(closestPoint);
            }
        }

        if (result->empty())
        {
            return result;
        }

        // Now solve ADSIL trilateration
        return solveAdsilTrilateration(tofMatrix);
    }

    std::shared_ptr<math::PointCloud> SignalSolver::filterPointsByFov(
        const std::shared_ptr<math::PointCloud> &allPoints,
        const std::shared_ptr<Device> &transmitter,
        const std::shared_ptr<Device> &receiver) const
    {
        // Filter points inside transmitter FOV first
        auto inTxFov = transmitter->pointsInFov(*allPoints);
        if (!inTxFov || inTxFov->empty())
        {
            return std::make_shared<PointCloud>();
        }

        // Then filter by receiver FOV
        auto inRxFov = receiver->pointsInFov(*inTxFov);
        return inRxFov ? inRxFov : std::make_shared<PointCloud>();
    }

    math::Point SignalSolver::findClosestPointInFov(
        const std::shared_ptr<math::PointCloud> &points,
        const std::shared_ptr<Device> &transmitter,
        const std::shared_ptr<Device> &receiver) const
    {
        if (!points || points->empty())
        {
            throw std::runtime_error("No points provided to find closest point");
        }

        float minDistance = std::numeric_limits<float>::max();
        Point closestPoint;

        const auto &txPosition = transmitter->getTransformNode()->getGlobalTransform().getPosition();
        const auto &rxPosition = receiver->getTransformNode()->getGlobalTransform().getPosition();

        for (const auto &point : points->getPoints())
        {
            float txDistance = point.distanceTo(txPosition);
            float rxDistance = point.distanceTo(rxPosition);
            float totalDistance = txDistance + rxDistance;

            if (totalDistance < minDistance)
            {
                minDistance = totalDistance;
                closestPoint = point;
            }
        }

        return closestPoint;
    }

    bool SignalSolver::isValidTofRow(const TofMatrix &tofMatrix, size_t txIndex) const
    {
        if (tofMatrix.rxCount != REQUIRED_RECEIVER_COUNT)
        {
            return false;
        }

        for (size_t rxIndex = 0; rxIndex < REQUIRED_RECEIVER_COUNT; ++rxIndex)
        {
            if (tofMatrix(txIndex, rxIndex) <= EPSILON)
            {
                return false;
            }
        }

        return true;
    }

    std::pair<math::Point, math::Point> SignalSolver::calculateAdsilPositions(
        const TofMatrix &tofMatrix,
        size_t txIndex,
        const SharedVec<Device> &receivers) const
    {
        // Calculate relative distances
        float R0 = tofMatrix(txIndex, 0) / 2.0f;
        float R1 = tofMatrix(txIndex, 1) - R0;
        float R2 = tofMatrix(txIndex, 2) - R0;
        float R3 = tofMatrix(txIndex, 3) - R0;

        // Get receiver positions (using receivers 1, 2, 3 for the calculation)
        auto c1 = receivers[1]->getGlobalTransform().getPosition();
        auto c2 = receivers[2]->getGlobalTransform().getPosition();
        auto c3 = receivers[3]->getGlobalTransform().getPosition();

        // Create coordinate system
        Vector P1P2 = c2.toVectorFrom(c1);
        float d = std::sqrt(P1P2.dot(P1P2));

        if (d < EPSILON)
        {
            throw std::runtime_error("Receivers P1 and P2 are too close together");
        }

        Vector ex = P1P2 * (1.0f / d); // normalize

        Vector c1c3 = c3.toVectorFrom(c1);
        float i = ex.dot(c1c3);

        Vector temp = c1c3 - (ex * i);
        float tempLengthSquared = temp.dot(temp);

        if (tempLengthSquared < EPSILON)
        {
            throw std::runtime_error("Receivers are collinear");
        }

        Vector ey = temp.normalized();
        Vector ez = ex.cross(ey);
        float j = ey.dot(c1c3);

        // Trilateration calculations
        float x = (R1 * R1 - R2 * R2 + d * d) / (2.0f * d);
        float y_numerator = R1 * R1 - R3 * R3 + i * i + j * j - 2.0f * i * x;
        float y = y_numerator / (2.0f * j);

        float zSquared = R1 * R1 - x * x - y * y;

        if (zSquared < 0.0f)
        {
            throw std::runtime_error("Invalid trilateration solution");
        }

        float z = std::sqrt(zSquared);

        // Calculate both possible solutions
        Vector resultVec1 = (ex * x) + (ey * y) + (ez * z);
        Vector resultVec2 = (ex * x) + (ey * y) + (ez * (-z));

        Point result1 = c1 + resultVec1;
        Point result2 = c1 + resultVec2;

        return std::make_pair(result1, result2);
    }

    std::shared_ptr<math::PointCloud> SignalSolver::solveAdsilTrilateration(const TofMatrix &tofMatrix)
    {
        auto result = std::make_shared<PointCloud>();

        if (tofMatrix.rxCount != REQUIRED_RECEIVER_COUNT)
        {
            throw std::runtime_error("ADSIL requires exactly 4 receivers");
        }

        const auto &transmitters = scene_->getTransmitters();
        const auto &receivers = scene_->getReceivers();

        for (size_t txIndex = 0; txIndex < tofMatrix.txCount; ++txIndex)
        {
            if (!isValidTofRow(tofMatrix, txIndex))
            {
                continue; // Skip invalid ToF measurements
            }

            try
            {
                auto [point1, point2] = calculateAdsilPositions(tofMatrix, txIndex, receivers);

                // Create possible solutions
                auto possiblePoints = std::make_shared<PointCloud>();
                possiblePoints->addPoint(point1);
                possiblePoints->addPoint(point2);

                // Filter by transmitter FOV to get valid solutions
                auto validSolutions = transmitters[txIndex]->pointsInFov(*possiblePoints);

                if (validSolutions && !validSolutions->empty())
                {
                    for (const auto &point : validSolutions->getPoints())
                    {
                        LOGGER_INFO("simulation", "From Transmitter: " + transmitters[txIndex]->getName());
                        LOGGER_INFO("simulation", "Detected ADSIL point: " + point.toString());

                        // Export point to CSV
                        utils::DataExporter::getInstance().exportPoint(
                            transmitters[txIndex]->getName(),
                            point.x(), point.y(), point.z());
                    }
                    result->addPoints(validSolutions->getPoints());
                }
            }
            catch (const std::runtime_error &e)
            {
                // Log error and continue with next transmitter
                LOGGER_INFO("simulation", "Skipping transmitter " + transmitters[txIndex]->getName() +
                                              ": " + std::string(e.what()));
            }
        }

        return result;
    }
}
