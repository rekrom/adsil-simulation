#include <device/Device.hpp>
#include <cmath>
#include <sstream>
#include <iostream>
constexpr float DEG_TO_RAD = static_cast<float>(M_PI) / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / static_cast<float>(M_PI);

Device::Device(const DeviceConfig &config)
    : transformNode_(std::make_shared<core::TransformNode>(config.transform)),
      vertical_fov_deg_(config.vertical_fov_deg),
      horizontal_fov_deg_(config.horizontal_fov_deg),
      vertical_fov_rad_(config.vertical_fov_deg * DEG_TO_RAD),
      horizontal_fov_rad_(config.horizontal_fov_deg * DEG_TO_RAD),
      name_(config.name) {}

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
        float epsilon = 1e-7;
        if ((std::abs(horizontal_p_angle - horizontal_d_angle) - (horizontal_fov_rad_ / 2.0f)) < epsilon &&
            (std::abs(vertical_p_angle - vertical_d_angle) - (vertical_fov_rad_ / 2.0f)) < epsilon)
        {
            visible->addPoint(point);
        }
    }

    return visible;
}

Transform Device::getGlobalTransform()
{
    return transformNode_->getGlobalTransform();
}

const Transform &Device::getTransform() const
{
    return transformNode_->getLocalTransform();
}

void Device::setTransform(const Transform &newTransform)
{
    transformNode_->setLocalTransform(newTransform);
}

const Point &Device::getOrigin() const
{
    return transformNode_->getLocalTransform().getPosition();
}

std::shared_ptr<core::TransformNode> Device::getTransformNode() const
{
    return transformNode_;
}

void Device::setTransformNode(std::shared_ptr<core::TransformNode> transformNode)
{
    transformNode_ = std::move(transformNode);
}

void Device::setOrigin(const Point &newOrigin)
{
    Transform t = transformNode_->getLocalTransform();
    t.setPosition(newOrigin);
    transformNode_->setLocalTransform(t);
}

const Vector Device::getDirection() const
{
    return transformNode_->getLocalTransform().get3DDirectionVector();
}

void Device::setDirection(const Vector &newDirection)
{
    Transform t = transformNode_->getLocalTransform();
    t.set3DDirectionVector(newDirection);
    transformNode_->setLocalTransform(t);
}

const float &Device::getVerticalFovDeg() const
{
    return vertical_fov_deg_;
}

void Device::setVerticalFovDeg(const float &verticalFovDeg)
{
    vertical_fov_rad_ = verticalFovDeg * DEG_TO_RAD;
}

const float &Device::getHorizontalFovDeg() const
{
    return horizontal_fov_deg_;
}
void Device::setHorizontalFovDeg(const float &horizontalFovDeg)
{
    horizontal_fov_rad_ = horizontalFovDeg * DEG_TO_RAD;
}

std::string Device::toString() const
{
    std::ostringstream oss;
    oss << "Device(origin=" << transformNode_->getLocalTransform().getPosition().toString()
        << ", direction=" << transformNode_->getLocalTransform().get3DDirectionVector().toString()
        << ", vFOV=" << vertical_fov_deg_
        << ", hFOV=" << horizontal_fov_deg_ << ")";
    return oss.str();
}
