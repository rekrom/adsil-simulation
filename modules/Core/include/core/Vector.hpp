#pragma once
#include <core/Point.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

class Point;

class Vector
{
public:
    Vector();
    Vector(float x, float y, float z);
    ~Vector();

    float x() const { return x_; }
    float y() const { return y_; }
    float z() const { return z_; }

    Vector normalized() const;
    float dot(const Vector &other) const;
    Vector cross(const Vector &other) const;
    Point rotatePoint(const Point &point) const;

    Vector operator+(const Vector &other) const;
    Vector operator-(const Vector &other) const;
    Vector operator*(const Vector &other) const;
    Vector operator*(float scalar) const;

    glm::vec3 toGlmVec3() const
    {
        return glm::vec3(x(), y(), z());
    }
    glm::quat toGlmQuat() const
    {
        // Assuming angles are in radians
        glm::quat q = glm::quat(glm::vec3(x(), y(), z())); // glm uses pitch, yaw, roll order internally
        return q;

        // Alternatively, explicit Euler to quat conversion:
        /*
        float cy = cos(z() * 0.5f);
        float sy = sin(z() * 0.5f);
        float cp = cos(y() * 0.5f);
        float sp = sin(y() * 0.5f);
        float cr = cos(x() * 0.5f);
        float sr = sin(x() * 0.5f);

        glm::quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
        */
    }

    static Vector fromGlmQuat(const glm::quat &q)
    {
        glm::vec3 euler = glm::eulerAngles(q);
        return Vector(euler.x, euler.y, euler.z);
    }

    std::string toString() const;

private:
    float x_, y_, z_;
};
