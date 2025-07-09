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

    // Rule of Five compliance
    Vector(const Vector &) = default;
    Vector &operator=(const Vector &) = default;
    Vector(Vector &&) = default;
    Vector &operator=(Vector &&) = default;

    [[nodiscard]] float x() const { return x_; }
    [[nodiscard]] float y() const { return y_; }
    [[nodiscard]] float z() const { return z_; }

    [[nodiscard]] Vector normalized() const;
    [[nodiscard]] float dot(const Vector &other) const;
    [[nodiscard]] Vector cross(const Vector &other) const;
    [[nodiscard]] Point rotatePoint(const Point &point) const;

    Vector operator+(const Vector &other) const;
    Vector operator-(const Vector &other) const;
    Vector operator*(const Vector &other) const;
    Vector operator*(float scalar) const;

    [[nodiscard]] glm::vec3 toGlmVec3() const
    {
        return {x(), y(), z()};
    }
    [[nodiscard]] glm::quat toGlmQuat() const
    {
        // Assuming angles are in radians
        // NOLINTNEXTLINE(modernize-return-braced-init-list): Explicit constructor call is intended
        return glm::quat(glm::vec3{x(), y(), z()});

        // Alternatively, explicit Euler to quat conversion:
        /*
        float cy = cos(z() * 0.5F);
        float sy = sin(z() * 0.5F);
        float cp = cos(y() * 0.5F);
        float sp = sin(y() * 0.5F);
        float cr = cos(x() * 0.5F);
        float sr = sin(x() * 0.5F);

        glm::quat q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;
        return q;
        */
    }

    [[nodiscard]] static Vector fromGlmQuat(const glm::quat &q)
    {
        glm::vec3 euler = glm::eulerAngles(q);
        // Access components via indices instead of .x/.y/.z to avoid Clang-Tidy union access warnings.
        const float x = euler[0];
        const float y = euler[1];
        const float z = euler[2];
        return Vector{x, y, z};
    }

    [[nodiscard]] std::string toString() const;

private:
    float x_, y_, z_;
};
