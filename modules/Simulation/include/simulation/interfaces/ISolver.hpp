#pragma once

#include <memory>
#include <math/PointCloud.hpp>

namespace simulation
{
    class ISolver
    {
    public:
        virtual ~ISolver() = default;

        // Run the solver for the current scene state and return detected point cloud
        virtual std::shared_ptr<math::PointCloud> solve() = 0;
    };
}
