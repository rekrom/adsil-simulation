#include "Cylinder.hpp"
#include "RotationUtils.hpp"
#include <cmath>

Cylinder::Cylinder(const Point &origin, float height, float radius, const Vector &orientation)
    : origin_(origin), height_(height), radius_(radius), orientation_(orientation) {}

std::shared_ptr<PointCloud> Cylinder::surfaceMesh(int quality) const
{
    auto cloud = std::make_shared<PointCloud>();
    int circRes = std::max(8, quality);
    int heightRes = std::max(2, quality / 2);
    float halfHeight = height_ / 2.0f;

    // Top and bottom
    for (float z : {-halfHeight, halfHeight})
    {
        for (int i = 0; i < circRes; ++i)
        {
            float angle = 2 * M_PI * i / circRes;
            Vector local(radius_ * std::cos(angle), radius_ * std::sin(angle), z);
            Vector rotated = RotationUtils::rotateRPY(local, orientation_);
            cloud->addPoint(Point(
                origin_.x() + rotated.x(),
                origin_.y() + rotated.y(),
                origin_.z() + rotated.z()));
        }
    }

    // Side surface
    for (int i = 0; i < circRes; ++i)
    {
        float angle = 2 * M_PI * i / circRes;
        Vector base(radius_ * std::cos(angle), radius_ * std::sin(angle), -halfHeight);
        Vector top(radius_ * std::cos(angle), radius_ * std::sin(angle), +halfHeight);

        for (int j = 0; j < heightRes; ++j)
        {
            float t = static_cast<float>(j) / (heightRes - 1);
            Vector local = base + (top - base) * t;
            Vector rotated = RotationUtils::rotateRPY(local, orientation_);
            cloud->addPoint(Point(
                origin_.x() + rotated.x(),
                origin_.y() + rotated.y(),
                origin_.z() + rotated.z()));
        }
    }

    return cloud;
}

std::string Cylinder::toString() const
{
    return "Cylinder(radius=" + std::to_string(radius_) + ", height=" + std::to_string(height_) + ")";
}
