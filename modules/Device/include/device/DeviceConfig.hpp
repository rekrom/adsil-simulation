#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <string>

struct DeviceConfig
{
    Point origin;
    Vector direction;
    float vertical_fov_deg;
    float horizontal_fov_deg;
    std::string name;
};
