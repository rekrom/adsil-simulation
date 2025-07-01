#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <core/Transform.hpp>
#include <string>

struct DeviceConfig
{
    Transform transform;
    float vertical_fov_deg;
    float horizontal_fov_deg;
    std::string name;
};
