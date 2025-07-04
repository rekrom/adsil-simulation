#include <geometry/implementations/Cylinder.hpp>
#include <core/RotationUtils.hpp>
#include <cmath>

Cylinder::Cylinder(const Transform &transform, float height, float radius)
    : ShapeBase(transform), cylinderDimension(height, radius) {}

std::shared_ptr<PointCloud> Cylinder::surfaceMesh(int quality) const
{
    auto cloud = std::make_shared<PointCloud>();
    int circRes = std::max(8, quality);
    int heightRes = std::max(2, quality / 2);
    float halfHeight = cylinderDimension.height_ / 2.0f;

    // Top and bottom
    for (float z : {-halfHeight, halfHeight})
    {
        for (int i = 0; i < circRes; ++i)
        {
            float angle = 2 * M_PI * i / circRes;
            Vector local(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), z);
            Vector rotated = RotationUtils::rotateRPY(local, transform_.getOrientation());
            cloud->addPoint(Point(
                transform_.getPosition().x() + rotated.x(),
                transform_.getPosition().y() + rotated.y(),
                transform_.getPosition().z() + rotated.z()));
        }
    }

    // Side surface
    for (int i = 0; i < circRes; ++i)
    {
        float angle = 2 * M_PI * i / circRes;
        Vector base(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), -halfHeight);
        Vector top(cylinderDimension.radius_ * std::cos(angle), cylinderDimension.radius_ * std::sin(angle), +halfHeight);

        for (int j = 0; j < heightRes; ++j)
        {
            float t = static_cast<float>(j) / (heightRes - 1);
            Vector local = base + (top - base) * t;
            Vector rotated = RotationUtils::rotateRPY(local, transform_.getOrientation());
            cloud->addPoint(Point(
                transform_.getPosition().x() + rotated.x(),
                transform_.getPosition().y() + rotated.y(),
                transform_.getPosition().z() + rotated.z()));
        }
    }

    return cloud;
}

std::vector<Point> Cylinder::wireframe() const
{
    std::vector<Point> framePoints;

    int segments = 16;
    float angleStep = 2 * M_PI / segments;
    float halfHeight = cylinderDimension.height_ / 2.0f;

    for (int i = 0; i < segments; ++i)
    {
        float angle = i * angleStep;
        float x = cylinderDimension.radius_ * cos(angle);
        float y = cylinderDimension.radius_ * sin(angle);

        Vector bottomLocal(x, y, -halfHeight);
        Vector topLocal(x, y, +halfHeight);

        Vector bottomRotated = RotationUtils::rotateRPY(bottomLocal, transform_.getOrientation());
        Vector topRotated = RotationUtils::rotateRPY(topLocal, transform_.getOrientation());

        framePoints.emplace_back(Point(
            transform_.getPosition().x() + bottomRotated.x(),
            transform_.getPosition().y() + bottomRotated.y(),
            transform_.getPosition().z() + bottomRotated.z()));
        framePoints.emplace_back(Point(
            transform_.getPosition().x() + topRotated.x(),
            transform_.getPosition().y() + topRotated.y(),
            transform_.getPosition().z() + topRotated.z()));
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
