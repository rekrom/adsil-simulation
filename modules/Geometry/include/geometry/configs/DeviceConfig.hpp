#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/Transform.hpp>
#include <string>

struct DeviceConfig
{
    spatial::Transform transform;
    float vertical_fov_deg;
    float horizontal_fov_deg;
    std::string name;
};
