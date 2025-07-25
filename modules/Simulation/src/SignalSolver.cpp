#include <simulation/SignalSolver.hpp>
#include <iostream>

SignalSolver::SignalSolver(std::shared_ptr<SimulationScene> scene)
    : scene_(std::move(scene))
{
}

std::shared_ptr<math::PointCloud> SignalSolver::solve()
{

    // std::cout << "solving..." << std::endl;
    auto result = std::make_shared<math::PointCloud>();
    auto allPoints = scene_->getMergedPointCloud();

    const auto &txs = scene_->getTransmitters();
    const auto &rxs = scene_->getReceivers();

    size_t tx_size = txs.size();
    size_t rx_size = rxs.size();

    std::vector<std::vector<float>> ToFVals(tx_size, std::vector<float>(rx_size, 0.0f));

    int tx_no = -1;
    for (const auto &tx : txs)
    {
        tx_no++;
        int rx_no = -1;
        for (const auto &rx : rxs)
        {
            rx_no++;

            // Filter points inside both FOVs
            auto inTxFov = tx->pointsInFov(*allPoints);
            if (!inTxFov || inTxFov->size() == 0)
            {
                continue;
            }

            auto inRxFov = rx->pointsInFov(*inTxFov);
            if (!inRxFov || inRxFov->size() == 0)
            {
                continue;
            }

            std::vector<math::Point> points = inRxFov->getPoints();

            // Find the closest point (based on Tx + Rx distance)
            float minDist = std::numeric_limits<float>::max();
            Point closest;

            for (const auto &pt : inRxFov->getPoints())
            {
                float dTx = pt.distanceTo(tx->getTransformNode()->getGlobalTransform().getPosition());
                float dRx = pt.distanceTo(rx->getTransformNode()->getGlobalTransform().getPosition());
                float total = dTx + dRx;

                if (total < minDist)
                {
                    minDist = total;
                    closest = pt;
                }
            }
            ToFVals[tx_no][rx_no] = minDist;
            // std::cout << "tofvals[" << tx_no << "][" << rx_no << "] = " << minDist << std::endl;
            // std::cout << solveCount_ << " " << tx->getName() << "-" << rx->getName() << "\t" << closest.toString() << "\t tof:" << minDist << std::endl;
            solveCount_++;
            result->addPoint(closest);
            result->clear();
            result->addPoints(points);
        }
    }
    return result;
    // return ToFVals

    // seperate the func
    // std::shared_ptr<math::PointCloud> SignalSolver::TofVals2AdsilPoints(const std::vector<std::vector<float>> &ToFVals)
    if (result->empty())
    {
        return result;
    }

    auto adsil_result = std::make_shared<math::PointCloud>();
    if (rx_size != 4)
    {
        throw std::runtime_error("rx_size must be 4");
    }

    tx_no = -1;
    for (const auto &tx : txs)
    {
        tx_no++;
        int rx_no = -1;

        // check the tof vals values are bigger than 0
        if (ToFVals[tx_no][0] <= 0.0f || ToFVals[tx_no][1] <= 0.0f || ToFVals[tx_no][2] <= 0.0f || ToFVals[tx_no][3] <= 0.0f)
        {
            continue; // Skip this Tx if any ToF value is not valid
        }

        float R0 = ToFVals[tx_no][0] / 2;
        float R1 = ToFVals[tx_no][1] - R0;
        float R2 = ToFVals[tx_no][2] - R0;
        float R3 = ToFVals[tx_no][3] - R0;

        auto c1 = rxs[1]->getGlobalTransform().getPosition();
        auto c2 = rxs[2]->getGlobalTransform().getPosition();
        auto c3 = rxs[3]->getGlobalTransform().getPosition();

        Vector P1P2 = c2.toVectorFrom(c1);
        float d = std::sqrt(P1P2.dot(P1P2));
        if (d == 0.0f)
        {
            continue; // Skip this Tx if P1 and P2 are the same
        }
        Vector ex = P1P2 * (1.0f / d); // normalize
        // i = ex • (c3 - c1)
        Vector c1c3 = c3.toVectorFrom(c1);
        float i = ex.dot(c1c3);

        // ey = normalized(c3 - c1 - i * ex)
        Vector temp = c1c3 - (ex * i);
        float tempLengthSquared = temp.dot(temp);
        if (tempLengthSquared == 0.0f)
        {
            continue; // Skip this Tx if P1 and P3 are the same
        }
        Vector ey = temp.normalized();

        // ez = ex × ey
        Vector ez = ex.cross(ey);

        // j = ey • (c3 - c1)
        float j = ey.dot(c1c3);

        // x = (r1² - r2² + d²) / (2d)
        float x = (R1 * R1 - R2 * R2 + d * d) / (2.0f * d);

        // y = (r1² - r3² + i² + j² - 2ix) / (2j)
        float y_numerator = R1 * R1 - R3 * R3 + i * i + j * j - 2.0f * i * x;
        float y = y_numerator / (2.0f * j);

        // z² = r1² - x² - y²
        float zSquared = R1 * R1 - x * x - y * y;
        if (zSquared < 0.0f)
        {
            continue; // Skip this Tx if the point is not valid
        }

        float z = std::sqrt(zSquared);

        float z1 = std::sqrt(zSquared);
        float z2 = -z1;

        // Final point = c1 + x * ex + y * ey + z * ez
        Vector resultVec = (ex * x) + (ey * y) + (ez * z1);
        Point result_point1 = c1 + resultVec;
        resultVec = (ex * x) + (ey * y) + (ez * z2);
        Point result_point2 = c1 + resultVec;
        auto possible_points = std::make_shared<math::PointCloud>();
        possible_points->clear();
        possible_points->addPoint(result_point1);
        possible_points->addPoint(result_point2);

        auto inFovSolution = tx->pointsInFov(*possible_points); // Ensure the point is within the Tx FOV

        adsil_result->addPoints(inFovSolution->getPoints());
    }
    return adsil_result;
}
