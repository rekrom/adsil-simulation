#include "Device.hpp"
#include <cmath>
#include <sstream>

constexpr float DEG_TO_RAD = static_cast<float>(M_PI) / 180.0f;

Device::Device(const DeviceConfig &config)
    : origin_(config.origin),
      direction_(config.direction.normalized()),
      vertical_fov_deg_(config.vertical_fov_deg),
      horizontal_fov_deg_(config.horizontal_fov_deg),
      vertical_fov_rad_(config.vertical_fov_deg * DEG_TO_RAD),
      horizontal_fov_rad_(config.horizontal_fov_deg * DEG_TO_RAD),
      name_(config.name) {}

std::shared_ptr<PointCloud> Device::pointsInFov(const std::shared_ptr<PointCloud> &pcd) const
{
    auto visible = std::make_shared<PointCloud>();

    for (const auto &point : pcd->getPoints())
    {
        Vector vec_to_point = point.toVectorFrom(origin_);

        float horizontal_p_angle = std::atan2(vec_to_point.y(), vec_to_point.x());
        float vertical_p_angle = std::atan2(vec_to_point.y(), vec_to_point.z());

        float horizontal_d_angle = std::atan2(direction_.y(), direction_.x());
        float vertical_d_angle = std::atan2(direction_.y(), direction_.z());

        if (std::abs(horizontal_p_angle - horizontal_d_angle) <= horizontal_fov_rad_ / 2.0f &&
            std::abs(vertical_p_angle - vertical_d_angle) <= vertical_fov_rad_ / 2.0f)
        {
            visible->addPoint(point);
        }
    }

    return visible;
}

const Point &Device::getOrigin() const
{
    return origin_;
}

void Device::setOrigin(const Point &newOrigin)
{
    origin_ = newOrigin;
}

const Vector &Device::getDirection() const
{
    return direction_;
}

void Device::setDirection(const Vector &newDirection)
{
    direction_ = newDirection.normalized(); // always keep direction normalized
}

std::string Device::toString() const
{
    std::ostringstream oss;
    oss << "Device(origin=" << origin_.toString()
        << ", direction=" << direction_.toString()
        << ", vFOV=" << vertical_fov_deg_
        << ", hFOV=" << horizontal_fov_deg_ << ")";
    return oss.str();
}
