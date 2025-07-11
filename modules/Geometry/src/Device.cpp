#include <geometry/implementations/Device.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

Device::Device(const DeviceConfig &config)
    : vertical_fov_rad_(RotationUtils::deg2rad(config.vertical_fov_deg)),
      horizontal_fov_rad_(RotationUtils::deg2rad(config.horizontal_fov_deg)),
      range_(config.range),
      name_(config.name)
{
    setTransformNode(std::make_shared<spatial::TransformNode>(config.transform));
}

std::shared_ptr<PointCloud> Device::pointsInFov(const PointCloud &pcd) const
{
    auto visible = std::make_shared<PointCloud>();
    const auto &globalTransform = transformNode_->getGlobalTransform();

    Point origin = globalTransform.getPosition();
    Vector direction = globalTransform.get3DDirectionVector();

    float horizontal_d_angle = std::atan2(direction.x(), direction.z()); // now Z is forward
    float vertical_d_angle = std::atan2(direction.y(), direction.z());

    float epsilon = static_cast<float>(1e-7);

    for (const auto &point : pcd.getPoints())
    {
        Vector vec_to_point = point.toVectorFrom(origin);

        float horizontal_p_angle = std::atan2(vec_to_point.x(), vec_to_point.z()); // horizontal in XZ plane
        float vertical_p_angle = std::atan2(vec_to_point.y(), vec_to_point.z());   // vertical in YZ plane

        if ((std::abs(horizontal_p_angle - horizontal_d_angle) - (horizontal_fov_rad_ / 2.0F)) < epsilon &&
            (std::abs(vertical_p_angle - vertical_d_angle) - (vertical_fov_rad_ / 2.0F)) < epsilon)
        {
            std::cout << getName() << " origin at " << origin.toString() << std::endl;
            visible->addPoint(point);
        }
    }

    return visible;
}

float Device::getHorizontalFovDeg() const
{
    return RotationUtils::rad2deg(horizontal_fov_rad_);
}

void Device::setHorizontalFovDeg(float horizontalFovDeg)
{
    horizontal_fov_rad_ = RotationUtils::deg2rad(horizontalFovDeg);
}

float Device::getVerticalFovDeg() const
{
    return RotationUtils::rad2deg(vertical_fov_rad_);
}

void Device::setVerticalFovDeg(float verticalFovDeg)
{
    vertical_fov_rad_ = RotationUtils::deg2rad(verticalFovDeg);
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
    oss << "Device(origin=" << transformNode_->getLocalTransform().getPosition().toString()
        << ", direction=" << transformNode_->getLocalTransform().get3DDirectionVector().toString()
        << ", vFOV=" << RotationUtils::rad2deg(vertical_fov_rad_)
        << ", hFOV=" << RotationUtils::rad2deg(horizontal_fov_rad_) << ")";
    return oss.str();
}
