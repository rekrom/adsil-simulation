#pragma once

#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <sstream>

struct CubeDimension final
{
    float length;
    float width;
    float height;

    constexpr CubeDimension(float dim = 0.0F)
        : length(dim), width(dim), height(dim) {}

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "CubeDimension(" << length << ", " << width << ", " << height << ")";
        return oss.str();
    }
};

struct CubeConfig
{
    spatial::Transform transform;
    CubeDimension dimension;
    std::string name;
};
