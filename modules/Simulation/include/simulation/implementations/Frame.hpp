#pragma once

#include <core/PointCloud.hpp>
#include <vector>
#include <string>
#include <memory>

namespace simulation
{
    struct Frame
    {
        std::shared_ptr<PointCloud> cloud;
        double timestamp = 0.0;
        std::vector<float> linearAcceleration; // imu
        std::vector<float> angularVelocity;
        std::string filePath;

        void clear()
        {
            if (cloud)
            {
                cloud.reset();
            }
            linearAcceleration.clear();
            angularVelocity.clear();
            filePath.clear();
        }
    };
}