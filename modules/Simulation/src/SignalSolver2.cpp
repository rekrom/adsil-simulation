#include <simulation/SignalSolver2.hpp>
#include <core/Logger.hpp>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>

using namespace math;

namespace simulation
{
    SignalSolver2::SignalSolver2(std::shared_ptr<SimulationScene> scene)
        : scene_(std::move(scene))
    {
    }

    std::shared_ptr<math::PointCloud> SignalSolver2::solve()
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

        // Pre-allocate ToF matrix
        TofMatrix tofMatrix(transmitters.size(), receivers.size());

        // Cache receiver positions once per solve
        std::vector<math::Point> rxPositions;
        rxPositions.reserve(receivers.size());
        for (const auto &rx : receivers)
        {
            rxPositions.emplace_back(rx->getTransformNode()->getGlobalTransform().getPosition());
        }

        // For each transmitter: prefilter by Tx FOV once
        for (size_t txIndex = 0; txIndex < transmitters.size(); ++txIndex)
        {
            const auto &tx = transmitters[txIndex];
            const math::Point txPos = tx->getTransformNode()->getGlobalTransform().getPosition();

            // Pre-filter points by transmitter FOV (single pass)
            auto inTxFov = tx->pointsInFov(*allPoints);
            if (!inTxFov || inTxFov->empty())
            {
                continue;
            }

            // For each receiver, filter by Rx FOV and pick closest
            for (size_t rxIndex = 0; rxIndex < receivers.size(); ++rxIndex)
            {
                const math::Point &rxPos = rxPositions[rxIndex];

                // Filter by receiver FOV on the already Tx-filtered set
                auto inRxFov = receivers[rxIndex]->pointsInFov(*inTxFov);
                if (!inRxFov || inRxFov->empty())
                {
                    continue;
                }

                // Find closest point to Tx+Rx (sum of distances)
                auto closestPoint = findClosestPointInSet(inRxFov, txPos, rxPos);
                float totalDistance = closestPoint.distanceTo(txPos) + closestPoint.distanceTo(rxPos);

                tofMatrix(txIndex, rxIndex) = totalDistance;
                solveCount_++;
                result->addPoint(closestPoint);
            }
        }

        if (result->empty())
        {
            return result;
        }

        // Solve ADSIL trilateration with computed ToF
        return solveAdsilTrilateration(tofMatrix);
    }

    inline math::Point SignalSolver2::findClosestPointInSet(const std::shared_ptr<math::PointCloud> &points,
                                                            const math::Point &txPos,
                                                            const math::Point &rxPos) const
    {
        float minDistance = std::numeric_limits<float>::max();
        math::Point closestPoint;

        for (const auto &point : points->getPoints())
        {
            float totalDistance = point.distanceTo(txPos) + point.distanceTo(rxPos);
            if (totalDistance < minDistance)
            {
                minDistance = totalDistance;
                closestPoint = point;
            }
        }

        return closestPoint;
    }

    bool SignalSolver2::isValidTofRow(const TofMatrix &tofMatrix, size_t txIndex) const
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

    std::pair<math::Point, math::Point> SignalSolver2::calculateAdsilPositions(
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

    std::shared_ptr<math::PointCloud> SignalSolver2::solveAdsilTrilateration(const TofMatrix &tofMatrix)
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
                    result->addPoints(validSolutions->getPoints());
                }
            }
            catch (const std::runtime_error &)
            {
                // Skip this transmitter on numeric issues
            }
        }

        return result;
    }
}