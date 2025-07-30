#include <simulation/SignalSolver.hpp>
#include <iostream>

SignalSolver::SignalSolver(std::shared_ptr<SimulationScene> scene)
    : scene_(std::move(scene))
{
    observerName_ = "SignalSolverObserver";
}

void SignalSolver::onFrameChanged(const std::shared_ptr<simulation::Frame> &frame)
{
    LOGGER_INFO("SignalSolver received frame change notification");
    if (!frame)
    {
        LOGGER_WARN("SignalSolver: Received empty frame");
        return;
    }

    // // id like to solve the signal for the current frame
    // // but we need to ensure the scene is updated first
    // scene_->onFrameChanged(frame);
    // // std::cout << "SignalSolver: Frame changed, solving..." << std::endl;
    // auto result = solve();
    // if (result)
    // {
    //     LOGGER_INFO("SignalSolver: Solved signal with " + std::to_string(result->size()) + " points");
    // }
    // else
    // {
    //     LOGGER_WARN("SignalSolver: No points found in the solution");
    // }
}

std::shared_ptr<math::PointCloud> SignalSolver::solve()
{
    // std::cout << "solving..." << std::endl;
    auto result = std::make_shared<math::PointCloud>();
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
            // std::cout << solveCount_ << " " << tx->getName() << "-" << rx->getName() << "\t" << closest.toString() << "\t tof:" << minDist << std::endl;
            solveCount_++;
            result->addPoint(closest);
        }
    }

    return result;
}
