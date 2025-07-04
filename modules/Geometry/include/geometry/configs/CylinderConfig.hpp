#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
struct CylinderDimension final
{
    float height_;
    float radius_;

    constexpr CylinderDimension(float height = 0.0f, float radius = 0.0f)
        : height_(height), radius_(radius) {}
};
struct CylinderConfig
{
    Point origin;
    float height;
    float radius;
    Vector orientation;
};
