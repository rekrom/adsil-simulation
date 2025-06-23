#pragma once

#include <string>
#include <cmath>
#include <core/Vector.hpp>
#include <glm/glm.hpp>

class Point
{
public:
    Point();
    Point(float x, float y, float z);
    ~Point();

    float x() const { return x_; }
    float y() const { return y_; }
    float z() const { return z_; }

    float distanceTo(const Point &other) const;
    Vector toVectorFrom(const Point &origin) const;

    glm::vec3 toGlmVec3() const
    {
        return glm::vec3(x(), y(), z());
    }

    Point operator+(const Vector &other) const;
    Point operator-(const Vector &other) const;

    Point operator+(const Point &other) const;
    Point operator-(const Point &other) const;

    std::string toString() const;

private:
    float x_, y_, z_;
};
