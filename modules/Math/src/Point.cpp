#include <math/Point.hpp>
#include <sstream>

namespace math
{
    Point::Point()
        : x_(0), y_(0), z_(0) {}

    Point::~Point()
    {
    }

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

    Point Point::operator+(const Vector &other) const
    {
        return Point(x_ + other.x(), y_ + other.y(), z_ + other.z());
    }

    Point Point::operator-(const Vector &other) const
    {
        return Point(x_ - other.x(), y_ - other.y(), z_ - other.z());
    }

    Point Point::operator+(const Point &other) const
    {
        return Point(x_ + other.x_, y_ + other.y_, z_ + other.z_);
    }

    Point Point::operator-(const Point &other) const
    {
        return Point(x_ - other.x_, y_ - other.y_, z_ - other.z_);
    }
}
