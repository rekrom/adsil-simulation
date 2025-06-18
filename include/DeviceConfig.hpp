#pragma once

#include "Point.hpp"
#include "Vector.hpp"
#include <string>

struct DeviceConfig
{
    Point origin;
    Vector direction;
    float vertical_fov_deg;
    float horizontal_fov_deg;
    std::string name;
};
