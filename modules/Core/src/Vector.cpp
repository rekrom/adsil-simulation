#include <core/Vector.hpp>
#include <cmath>
#include <sstream>

Vector::Vector()
    : x_(0), y_(0), z_(0) {}

Vector::Vector(float x, float y, float z)
    : x_(x), y_(y), z_(z) {}

Vector::~Vector()
{
}

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

Point Vector::rotatePoint(const Point &point) const
{
    glm::quat q = toGlmQuat();
    glm::vec3 p = point.toGlmVec3();

    glm::vec3 rotated = q * p; // quaternion-vector multiplication

    return Point(rotated.x, rotated.y, rotated.z);
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

Vector Vector::operator*(const Vector &other) const
{
    glm::quat q1 = this->toGlmQuat();
    glm::quat q2 = other.toGlmQuat();
    glm::quat q = q1 * q2;
    return Vector::fromGlmQuat(q);
}

std::string Vector::toString() const
{
    std::ostringstream oss;
    oss << "Vector(" << x_ << ", " << y_ << ", " << z_ << ")";
    return oss.str();
}
