#include <simulation/SignalSolver.hpp>
#include <iostream>

SignalSolver::SignalSolver(std::shared_ptr<SimulationScene> scene)
    : scene_(std::move(scene))
{
}

std::shared_ptr<PointCloud> SignalSolver::solve()
{
    // std::cout << "solving..." << std::endl;
    auto result = std::make_shared<PointCloud>();
    auto allPoints = scene_->getMergedPointCloud();

    std::cout << "solving..." << std::endl;
    for (const auto &p : allPoints->getPoints())
    {
        std::cout << p.toString() << std::endl;
    }

    // TEST PURPOSES CLEAR THIS AREA
    // allPoints->clear();
    // allPoints->addPoint(Point(-500 / 1000.0F, 1200 / 1000.0F, 0));
    // allPoints->addPoint(Point(0, 1200 / 1000.0F, 0));
    //

    const auto &txs = scene_->getTransmitters();
    const auto &rxs = scene_->getReceivers();

    for (const auto &tx : txs)
    {
        for (const auto &rx : rxs)
        {
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

            std::vector<Point> points = inRxFov->getPoints();

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
            std::cout << tx->getName() << "-" << rx->getName() << "\t" << closest.toString() << "\t tof:" << minDist << std::endl;
            result->addPoint(closest);
        }
    }

    return result;
}
