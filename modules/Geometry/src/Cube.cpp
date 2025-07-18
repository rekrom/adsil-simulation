#include <geometry/implementations/Cube.hpp>
#include <core/RotationUtils.hpp>
#include <iostream>

Cube::Cube(CubeConfig config)
    : ShapeBase(config.name), cubeDimension_(config.dimension)
{
    setTransformNode(std::make_shared<spatial::TransformNode>(config.transform));
}

std::shared_ptr<PointCloud> Cube::surfaceMesh(int quality) const
{
    auto cloud = std::make_shared<PointCloud>();
    auto dim = cubeDimension_.height; // it can be width or length
    float half = dim / 2.0F;
    int n = std::max(2, static_cast<int>(std::sqrt(quality)));

    std::vector<std::tuple<Vector, Vector, Vector>> faceConfigs = {
        {Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)},
        {Vector(-1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)},
        {Vector(0, 1, 0), Vector(1, 0, 0), Vector(0, 0, 1)},
        {Vector(0, -1, 0), Vector(1, 0, 0), Vector(0, 0, 1)},
        {Vector(0, 0, 1), Vector(1, 0, 0), Vector(0, 1, 0)},
        {Vector(0, 0, -1), Vector(1, 0, 0), Vector(0, 1, 0)},
    };

    std::cout << "==================Meshing STARTED==================" << std::endl;

    for (const auto &[normal, u, v] : faceConfigs)
    {
        auto facePoints = generateFace(normal * half, u, v, n);
        for (const auto &p : facePoints)
        {
            cloud->addPoint(p);
        }
    }

    // for (const auto &p : cloud->getPoints())
    // {
    //     std::cout << p.toString() << std::endl;
    // }

    std::cout << "==================Meshing DONE==================" << std::endl;

    return cloud;
}

std::vector<Point> Cube::wireframe() const
{
    std::vector<Point> edges;
    auto node = getTransformNode();
    auto transform = node->getGlobalTransform();
    glm::vec3 center = transform.getPosition().toGlmVec3();
    auto dim = cubeDimension_.height; // it can be width or length
    float half = dim / 2.0F;

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

std::vector<Point> Cube::generateFace(const Vector &center, const Vector &u, const Vector &v, int n) const
{
    std::vector<Point> points;
    auto dim = cubeDimension_.height; // it can be width or length

    float step = dim / static_cast<float>(n - 1);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Vector offset = u * (-dim / 2 + static_cast<float>(i) * step) + v * (-dim / 2 + static_cast<float>(j) * step);
            auto node = getTransformNode();
            auto transform = node->getGlobalTransform();
            Vector rotated = RotationUtils::rotateRPY(center + offset, transform.getOrientation());
            points.emplace_back(
                transform.getPosition().x() + rotated.x(),
                transform.getPosition().y() + rotated.y(),
                transform.getPosition().z() + rotated.z());
        }
    }

    return points;
}

CubeDimension Cube::getDimension()
{
    return cubeDimension_;
}

std::string Cube::toString() const
{
    return "Cube(dim=" + cubeDimension_.toString() + ")";
}
