#include <geometry/implementations/Device.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

Device::Device(const DeviceConfig &config)
    : vertical_fov_rad_(RotationUtils::deg2rad(config.vertical_fov_deg)),
      horizontal_fov_rad_(RotationUtils::deg2rad(config.horizontal_fov_deg)),
      name_(config.name)
{
    setTransformNode(std::make_shared<spatial::TransformNode>(config.transform));

    range_ = 20.0F;
}

std::shared_ptr<PointCloud> Device::pointsInFov(const PointCloud &pcd) const
{
    auto visible = std::make_shared<PointCloud>();

    for (const auto &point : pcd.getPoints())
    {
        Vector vec_to_point = point.toVectorFrom(transformNode_->getLocalTransform().getPosition());
        float horizontal_p_angle = std::atan2(vec_to_point.y(), vec_to_point.x());
        float vertical_p_angle = std::atan2(vec_to_point.y(), vec_to_point.z());

        float horizontal_d_angle = std::atan2(transformNode_->getLocalTransform().get3DDirectionVector().y(), transformNode_->getLocalTransform().get3DDirectionVector().x());
        float vertical_d_angle = std::atan2(transformNode_->getLocalTransform().get3DDirectionVector().y(), transformNode_->getLocalTransform().get3DDirectionVector().z());
        float epsilon = static_cast<float>(1e-7);
        if ((std::abs(horizontal_p_angle - horizontal_d_angle) - (horizontal_fov_rad_ / 2.0F)) < epsilon &&
            (std::abs(vertical_p_angle - vertical_d_angle) - (vertical_fov_rad_ / 2.0F)) < epsilon)
        {
            visible->addPoint(point);
        }
    }

    return visible;
}

const Point &Device::getOrigin() const
{
    return transformNode_->getLocalTransform().getPosition();
}

const Vector &Device::getOrientation() const
{
    return transformNode_->getLocalTransform().getOrientation();
}

void Device::setOrientation(const Vector &newOrientation)
{
    spatial::Transform t = getTransformNode()->getLocalTransform();
    t.setOrientation(newOrientation); // assumes rpy = (roll, pitch, yaw)
    getTransformNode()->setLocalTransform(t);
}

void Device::setOrigin(const Point &newOrigin)
{
    spatial::Transform t = transformNode_->getLocalTransform();
    t.setPosition(newOrigin);
    transformNode_->setLocalTransform(t);
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
