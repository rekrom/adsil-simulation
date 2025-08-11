#include <geometry/implementations/Device.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

Device::Device(const DeviceConfig &config)
    : vertical_fov_rad_(math::RotationUtils::deg2rad(config.vertical_fov_deg)),
      horizontal_fov_rad_(math::RotationUtils::deg2rad(config.horizontal_fov_deg)),
      range_(config.range),
      name_(config.name)
{
    setTransformNode(std::make_shared<spatial::TransformNode>(config.transform));
}

std::shared_ptr<math::PointCloud> Device::pointsInFov(const math::PointCloud &pcd) const
{
    float fovH = this->getHorizontalFovRad();
    float fovV = this->getVerticalFovRad();
    float range = this->getRange();

    float halfW = range * tanf(fovH / math::constants::HALF_DIVISOR_F);
    float halfH = range * tanf(fovV / math::constants::HALF_DIVISOR_F);

    auto newPoint1 = std::make_shared<spatial::TransformNode>();
    auto newPoint2 = std::make_shared<spatial::TransformNode>();
    auto newPoint3 = std::make_shared<spatial::TransformNode>();
    auto newPoint4 = std::make_shared<spatial::TransformNode>();

    this->getTransformNode()->addChild(newPoint1);
    this->getTransformNode()->addChild(newPoint2);
    this->getTransformNode()->addChild(newPoint3);
    this->getTransformNode()->addChild(newPoint4);

    // Set local transforms for each FOV corner
    newPoint1->setLocalTransform(spatial::Transform({range, -halfW, halfH}, {0.0F, 0.0F, 0.0F}));
    newPoint2->setLocalTransform(spatial::Transform({range, halfW, halfH}, {0.0F, 0.0F, 0.0F}));
    newPoint3->setLocalTransform(spatial::Transform({range, halfW, -halfH}, {0.0F, 0.0F, 0.0F}));
    newPoint4->setLocalTransform(spatial::Transform({range, -halfW, -halfH}, {0.0F, 0.0F, 0.0F}));

    // Now get the global positions (which will include the transmitter's orientation)
    math::Point p1 = newPoint1->getGlobalTransform().getPosition();
    math::Point p2 = newPoint2->getGlobalTransform().getPosition();
    math::Point p3 = newPoint3->getGlobalTransform().getPosition();
    math::Point p4 = newPoint4->getGlobalTransform().getPosition();

    Point device_origin = this->getTransformNode()->getGlobalTransform().getPosition();

    Vector device_front = this->getTransformNode()->getGlobalTransform().get3DDirectionVector();

    math::Vector v1 = p1.toVectorFrom(device_origin);
    math::Vector v2 = p2.toVectorFrom(device_origin);
    math::Vector v3 = p3.toVectorFrom(device_origin);
    math::Vector v4 = p4.toVectorFrom(device_origin);

    auto visible = std::make_shared<math::PointCloud>();

    for (const auto &point : pcd.getPoints())
    {
        auto cornerPoint1 = math::helper::intersectLinePlane(point, device_front, device_origin, v1);
        auto cornerPoint2 = math::helper::intersectLinePlane(point, device_front, device_origin, v2);
        auto cornerPoint3 = math::helper::intersectLinePlane(point, device_front, device_origin, v3);
        auto cornerPoint4 = math::helper::intersectLinePlane(point, device_front, device_origin, v4);

        if (!cornerPoint1 || !cornerPoint2 || !cornerPoint3 || !cornerPoint4)
        {
            continue; // Skip points that do not intersect with the plane defined by the FOV corners
        }

        auto isInConvex = math::helper::isPointInConvexQuad(point, cornerPoint1.value(), cornerPoint2.value(), cornerPoint3.value(), cornerPoint4.value());
        if (isInConvex)
        {
            visible->addPoint(point);
        }
    }

    return visible;
}

float Device::getHorizontalFovDeg() const
{
    return math::RotationUtils::rad2deg(horizontal_fov_rad_);
}

void Device::setHorizontalFovDeg(float horizontalFovDeg)
{
    horizontal_fov_rad_ = math::RotationUtils::deg2rad(horizontalFovDeg);
}

float Device::getVerticalFovDeg() const
{
    return math::RotationUtils::rad2deg(vertical_fov_rad_);
}

void Device::setVerticalFovDeg(float verticalFovDeg)
{
    vertical_fov_rad_ = math::RotationUtils::deg2rad(verticalFovDeg);
}

float Device::getHorizontalFovRad() const
{
    return horizontal_fov_rad_;
}

void Device::setHorizontalFovRad(float horizontalFovRad)
{
    horizontal_fov_rad_ = horizontalFovRad;
}

float Device::getVerticalFovRad() const
{
    return vertical_fov_rad_;
}

void Device::setVerticalFovRad(float verticalFovRad)
{
    vertical_fov_rad_ = verticalFovRad;
}

const float &Device::getRange() const
{
    return range_;
}

void Device::setRange(float newRange)
{
    range_ = newRange;
}

std::string Device::toString() const
{
    std::ostringstream oss;
    oss << "Device(origin=" << transformNode_->getGlobalTransform().getPosition().toString()
        << ", direction=" << transformNode_->getGlobalTransform().get3DDirectionVector().toString()
        << ", vFOV=" << math::RotationUtils::rad2deg(vertical_fov_rad_)
        << ", hFOV=" << math::RotationUtils::rad2deg(horizontal_fov_rad_) << ")";
    return oss.str();
}
