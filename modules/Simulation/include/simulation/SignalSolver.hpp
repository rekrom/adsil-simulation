#pragma once

#include "SimulationScene.hpp"
#include <math/PointCloud.hpp>
#include <math/Point.hpp>

#include <vector>
#include <tuple>
#include <memory>
#include <simulation/interfaces/IFrameObserver.hpp>

class SignalSolver : public simulation::IFrameObserver
{
public:
    explicit SignalSolver(std::shared_ptr<SimulationScene> scene);
    void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override;

    // Runs the solver and returns closest points for each (Tx, Rx) pair
    std::shared_ptr<math::PointCloud> solve();

private:
    std::shared_ptr<SimulationScene> scene_;
    size_t solveCount_ = 0; // For debugging or tracking how many times solve is called
};
