#pragma once

#include "SimulationScene.hpp"
#include <math/math.hpp>

#include <vector>
#include <tuple>
#include <memory>
#include <simulation/interfaces/IFrameObserver.hpp>

class SignalSolver
{
public:
    explicit SignalSolver(std::shared_ptr<SimulationScene> scene);

    // Runs the solver and returns closest points for each (Tx, Rx) pair
    std::shared_ptr<math::PointCloud> solve();

private:
    std::shared_ptr<SimulationScene> scene_;
    size_t solveCount_ = 0; // For debugging or tracking how many times solve is called
};
