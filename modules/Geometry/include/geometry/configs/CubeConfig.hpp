#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>

struct CubeDimension final
{
    float length;
    float width;
    float height;

    constexpr CubeDimension(float dim = 0.0f)
        : length(dim), width(dim), height(dim) {}
};

struct CubeConfig
{
    Point origin;
    float dimension;
    Vector orientation;
};
