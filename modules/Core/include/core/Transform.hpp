#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>

class Transform
{
public:
    Transform();
    Transform(const Point &position, const Vector &orientation);

    const Point &getPosition() const;
    void setPosition(const Point &position);

    const Vector &getOrientation() const;
    void setOrientation(const Vector &orientation);

    void move(const Vector &delta);
    Vector get3DDirectionVector() const;

    void rotateYaw(float angleRad);

private:
    Point position_;
    Vector orientation_; // roll (x), pitch (y), yaw (z) — radyan cinsinden
};
