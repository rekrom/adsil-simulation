#include "Vector.hpp"
#include <cmath>
#include <sstream>

Vector::Vector(float x, float y, float z)
    : x_(x), y_(y), z_(z) {}

Vector Vector::normalized() const
{
    float mag = std::sqrt(x_ * x_ + y_ * y_ + z_ * z_);
    if (mag == 0.0f)
        return Vector(0.0f, 0.0f, 0.0f);
    return Vector(x_ / mag, y_ / mag, z_ / mag);
}

float Vector::dot(const Vector &other) const
{
    return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}

Vector Vector::cross(const Vector &other) const
{
    return Vector(
        y_ * other.z_ - z_ * other.y_,
        z_ * other.x_ - x_ * other.z_,
        x_ * other.y_ - y_ * other.x_);
}

Vector Vector::operator+(const Vector &other) const
{
    return Vector(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

Vector Vector::operator-(const Vector &other) const
{
    return Vector(x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

Vector Vector::operator*(float scalar) const
{
    return Vector(x_ * scalar, y_ * scalar, z_ * scalar);
}

std::string Vector::toString() const
{
    std::ostringstream oss;
    oss << "Vector(" << x_ << ", " << y_ << ", " << z_ << ")";
    return oss.str();
}
