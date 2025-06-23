#pragma once

#include "SimulationScene.hpp"
#include <core/PointCloud.hpp>
#include <core/Point.hpp>

#include <vector>
#include <tuple>
#include <memory>

class SignalSolver
{
public:
    explicit SignalSolver(std::shared_ptr<SimulationScene> scene);

    // Runs the solver and returns closest points for each (Tx, Rx) pair
    std::shared_ptr<PointCloud> solve();

private:
    std::shared_ptr<SimulationScene> scene_;
};
