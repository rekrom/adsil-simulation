#pragma once
#include <glm/glm.hpp>

#include <string>

class Vector
{
public:
    Vector(float x, float y, float z);

    float x() const { return x_; }
    float y() const { return y_; }
    float z() const { return z_; }

    Vector normalized() const;
    float dot(const Vector &other) const;
    Vector cross(const Vector &other) const;

    Vector operator+(const Vector &other) const;
    Vector operator-(const Vector &other) const;
    Vector operator*(float scalar) const;

    glm::vec3 toGlmVec3() const
    {
        return glm::vec3(x(), y(), z());
    }

    std::string toString() const;

private:
    float x_, y_, z_;
};
