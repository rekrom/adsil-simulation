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

    size_t tx_size = txs->size();
    size_t rx_size = rxs->size();
    std::vector<std::vector<float>> ToFVals(tx_size, std::vector<float>(rx_size, 0.0f));

    int tx_no=-1;
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
            // std::cout << solveCount_ << " " << tx->getName() << "-" << rx->getName() << "\t" << closest.toString() << "\t tof:" << minDist << std::endl;
            solveCount_++;
            result->addPoint(closest);
        }
    }
    //return ToFVals


    // seperate the func
    // std::shared_ptr<math::PointCloud> SignalSolver::TofVals2AdsilPoints(const std::vector<std::vector<float>> &ToFVals)
    auto result = std::make_shared<math::PointCloud>();





    return result;
}


{
    

}

