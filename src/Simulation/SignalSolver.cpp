#include "Simulation/SignalSolver.hpp"

SignalSolver::SignalSolver(std::shared_ptr<SimulationScene> scene)
    : scene_(std::move(scene)) {}

std::shared_ptr<PointCloud> SignalSolver::solve()
{
    auto result = std::make_shared<PointCloud>();
    auto allPoints = scene_->getMergedPointCloud();

    const auto &txs = scene_->getTransmitters();
    const auto &rxs = scene_->getReceivers();

    for (const auto &tx : txs)
    {
        for (const auto &rx : rxs)
        {
            // Filter points inside both FOVs
            auto inTxFov = tx->pointsInFov(*allPoints);
            if (!inTxFov || inTxFov->size() == 0)
                continue;

            auto inRxFov = rx->pointsInFov(*inTxFov);
            if (!inRxFov || inRxFov->size() == 0)
                continue;

            // Find the closest point (based on Tx + Rx distance)
            float minDist = std::numeric_limits<float>::max();
            Point closest;

            for (const auto &pt : inRxFov->getPoints())
            {
                float dTx = pt.distanceTo(tx->getOrigin());
                float dRx = pt.distanceTo(rx->getOrigin());
                float total = dTx + dRx;

                if (total < minDist)
                {
                    minDist = total;
                    closest = pt;
                }
            }

            result->addPoint(closest);
        }
    }

    return result;
}
