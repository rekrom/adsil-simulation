#include <geometry/implementations/Cylinder.hpp>
#include <math/RotationUtils.hpp>
#include <cmath>

Cylinder::Cylinder(CylinderConfig config)
    : ShapeBase(config.name), cylinderDimension(config.dimension)
{
    setTransformNode(std::make_shared<spatial::TransformNode>(config.transform));
}

std::shared_ptr<PointCloud> Cylinder::surfaceMesh(int quality) const
{
    auto cloud = std::make_shared<PointCloud>();
    int circRes = std::max(8, quality);
    int heightRes = std::max(2, quality / 2);
    float halfHeight = cylinderDimension.height_ / 2.0F;
    auto globalTransform = getGlobalTransform();
    // Top and bottom
    for (float z : {-halfHeight, halfHeight})
    {
        for (int i = 0; i < circRes; ++i)
        {
            constexpr float PI = static_cast<float>(M_PI);
            float angle = 2.0F * PI * static_cast<float>(i) / static_cast<float>(circRes);

            Vector local(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), z);
            Vector rotated = RotationUtils::rotateRPY(local, globalTransform.getOrientation());
            cloud->addPoint(Point(
                globalTransform.getPosition().x() + rotated.x(),
                globalTransform.getPosition().y() + rotated.y(),
                globalTransform.getPosition().z() + rotated.z()));
        }
    }

    // Side surface
    for (int i = 0; i < circRes; ++i)
    {
        constexpr float PI = static_cast<float>(M_PI);
        float angle = 2.0F * PI * static_cast<float>(i) / static_cast<float>(circRes);

        Vector base(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), -halfHeight);
        Vector top(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), +halfHeight);

        for (int j = 0; j < heightRes; ++j)
        {
            float t = static_cast<float>(j) / static_cast<float>(heightRes - 1);
            Vector local = base + (top - base) * t;
            auto node = getTransformNode();
            auto transform = node->getGlobalTransform();
            Vector rotated = RotationUtils::rotateRPY(local, transform.getOrientation());
            cloud->addPoint(Point(
                transform.getPosition().x() + rotated.x(),
                transform.getPosition().y() + rotated.y(),
                transform.getPosition().z() + rotated.z()));
        }
    }

    return cloud;
}

std::vector<Point> Cylinder::wireframe() const
{
    std::vector<Point> framePoints;

    int segments = 16;
    constexpr float PI = static_cast<float>(M_PI);
    float angleStep = 2.0F * PI / static_cast<float>(segments);
    float halfHeight = cylinderDimension.height_ / 2.0F;

    for (int i = 0; i < segments; ++i)
    {
        float angle = static_cast<float>(i) * angleStep;
        float x = cylinderDimension.radius_ * cosf(angle);
        float y = cylinderDimension.radius_ * sinf(angle);

        Vector bottomLocal(x, y, -halfHeight);
        Vector topLocal(x, y, +halfHeight);
        auto node = getTransformNode();
        auto transform = node->getGlobalTransform();
        Vector bottomRotated = RotationUtils::rotateRPY(bottomLocal, transform.getOrientation());
        Vector topRotated = RotationUtils::rotateRPY(topLocal, transform.getOrientation());

        framePoints.emplace_back(Point(
            transform.getPosition().x() + bottomRotated.x(),
            transform.getPosition().y() + bottomRotated.y(),
            transform.getPosition().z() + bottomRotated.z()));
        framePoints.emplace_back(Point(
            transform.getPosition().x() + topRotated.x(),
            transform.getPosition().y() + topRotated.y(),
            transform.getPosition().z() + topRotated.z()));
    }

    return framePoints;
}

float Cylinder::getRadius()
{
    return cylinderDimension.radius_;
}
float Cylinder::getHeight()
{
    return cylinderDimension.height_;
}

std::string Cylinder::toString() const
{
    return "Cylinder(radius=" + std::to_string(cylinderDimension.radius_) + ", height=" + std::to_string(cylinderDimension.height_) + ")";
}
