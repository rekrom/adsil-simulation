#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/Transform.hpp>

struct CylinderDimension final
{
    float height_;
    float radius_;

    constexpr CylinderDimension(float height = 0.0F, float radius = 0.0F)
        : height_(height), radius_(radius) {}
};
struct CylinderConfig
{
    spatial::Transform transform;
    CylinderDimension dimension;
    std::string name;
};
