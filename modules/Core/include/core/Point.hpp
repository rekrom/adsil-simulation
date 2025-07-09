#pragma once
#include <core/Vector.hpp>
#include <string>
#include <cmath>
#include <glm/glm.hpp>

class Vector;
class Point
{
public:
    Point();
    Point(float x, float y, float z);
    ~Point();

    // Rule of Five compliance
    Point(const Point &) = default;
    Point &operator=(const Point &) = default;
    Point(Point &&) = default;
    Point &operator=(Point &&) = default;

    [[nodiscard]] float x() const { return x_; }
    [[nodiscard]] float y() const { return y_; }
    [[nodiscard]] float z() const { return z_; }

    [[nodiscard]] float distanceTo(const Point &other) const;
    [[nodiscard]] Vector toVectorFrom(const Point &origin) const;

    [[nodiscard]] glm::vec3 toGlmVec3() const
    {
        return {x(), y(), z()};
    }

    Point operator+(const Vector &other) const;
    Point operator-(const Vector &other) const;

    Point operator+(const Point &other) const;
    Point operator-(const Point &other) const;

    [[nodiscard]] std::string toString() const;

private:
    float x_, y_, z_;
};
