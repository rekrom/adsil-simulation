#include "Geometry/Cube.hpp"
#include "Core/RotationUtils.hpp"

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
std::vector<Point> Cube::wireframe() const
{
    std::vector<Point> edges;
    glm::vec3 center = origin_.toGlmVec3();
    float half = dimension_ / 2.0f;

    // Cube köşe noktaları
    glm::vec3 corners[8] = {
        center + glm::vec3(-half, -half, -half),
        center + glm::vec3(half, -half, -half),
        center + glm::vec3(half, half, -half),
        center + glm::vec3(-half, half, -half),
        center + glm::vec3(-half, -half, half),
        center + glm::vec3(half, -half, half),
        center + glm::vec3(half, half, half),
        center + glm::vec3(-half, half, half),
    };

    auto p = [](const glm::vec3 &v)
    { return Point(v.x, v.y, v.z); };

    auto addEdge = [&](int a, int b)
    {
        edges.push_back(p(corners[a]));
        edges.push_back(p(corners[b]));
    };

    // 12 kenar
    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(2, 3);
    addEdge(3, 0);

    addEdge(4, 5);
    addEdge(5, 6);
    addEdge(6, 7);
    addEdge(7, 4);

    addEdge(0, 4);
    addEdge(1, 5);
    addEdge(2, 6);
    addEdge(3, 7);

    return edges;
}
#include <iostream>
std::vector<Point> Cube::generateFace(const Vector &center, const Vector &u, const Vector &v, int n) const
{
    std::vector<Point> points;
    float step = dimension_ / (n - 1);
    std::cout << "step: " << step << std::endl;

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
