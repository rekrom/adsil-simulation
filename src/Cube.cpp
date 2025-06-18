#include "Cube.hpp"
#include "RotationUtils.hpp"

Cube::Cube(const Point &origin, float dimension, const Vector &orientation)
    : origin_(origin), dimension_(dimension), orientation_(orientation) {}

std::shared_ptr<PointCloud> Cube::surfaceMesh(int quality) const
{
    auto cloud = std::make_shared<PointCloud>();
    float half = dimension_ / 2.0f;
    int n = std::max(2, static_cast<int>(std::sqrt(quality)));

    std::vector<std::tuple<Vector, Vector, Vector>> faceConfigs = {
        {Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)},
        {Vector(-1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)},
        {Vector(0, 1, 0), Vector(1, 0, 0), Vector(0, 0, 1)},
        {Vector(0, -1, 0), Vector(1, 0, 0), Vector(0, 0, 1)},
        {Vector(0, 0, 1), Vector(1, 0, 0), Vector(0, 1, 0)},
        {Vector(0, 0, -1), Vector(1, 0, 0), Vector(0, 1, 0)},
    };

    for (const auto &[normal, u, v] : faceConfigs)
    {
        auto facePoints = generateFace(normal * half, u, v, n);
        for (const auto &p : facePoints)
        {
            cloud->addPoint(p);
        }
    }

    return cloud;
}

std::vector<Point> Cube::generateFace(const Vector &center, const Vector &u, const Vector &v, int n) const
{
    std::vector<Point> points;
    float step = dimension_ / (n - 1);

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Vector offset = u * (-dimension_ / 2 + i * step) + v * (-dimension_ / 2 + j * step);
            Vector rotated = RotationUtils::rotateRPY(center + offset, orientation_);
            points.emplace_back(
                origin_.x() + rotated.x(),
                origin_.y() + rotated.y(),
                origin_.z() + rotated.z());
        }
    }

    return points;
}

std::string Cube::toString() const
{
    return "Cube(dim=" + std::to_string(dimension_) + ")";
}
