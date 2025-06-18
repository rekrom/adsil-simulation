#include "Point.hpp"
#include <sstream>

Point::Point(float x, float y, float z)
    : x_(x), y_(y), z_(z) {}

float Point::distanceTo(const Point &other) const
{
    float dx = x_ - other.x_;
    float dy = y_ - other.y_;
    float dz = z_ - other.z_;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

Vector Point::toVectorFrom(const Point &origin) const
{
    return Vector(x_ - origin.x(), y_ - origin.y(), z_ - origin.z());
}

std::string Point::toString() const
{
    std::ostringstream oss;
    oss << "Point(x=" << x_ << ", y=" << y_ << ", z=" << z_ << ")";
    return oss.str();
}

Point Point::operator+(const Vector &v) const
{
    return Point(x() + v.x(), y() + v.y(), z() + v.z());
}

Point Point::operator-(const Vector &v) const
{
    return Point(x() - v.x(), y() - v.y(), z() - v.z());
}