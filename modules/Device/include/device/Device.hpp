#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <core/TransformNode.hpp>
#include <core/PointCloud.hpp>
#include <string>
#include <memory>
#include <device/DeviceConfig.hpp>

class Device
{
public:
    Device(const DeviceConfig &config);

    std::shared_ptr<PointCloud> pointsInFov(const PointCloud &pcd) const;

    std::string toString() const;

    const std::string &getName() const { return name_; }

    Transform getGlobalTransform();

    const Transform &getTransform() const;
    void setTransform(const Transform &newTransform);

    std::shared_ptr<core::TransformNode> getTransformNode() const;
    void setTransformNode(std::shared_ptr<core::TransformNode> transformNode);

    const Point &getOrigin() const;
    void setOrigin(const Point &newOrigin);

    const Vector getDirection() const;
    void setDirection(const Vector &newDirection);

    const float &getVerticalFovDeg() const;
    void setVerticalFovDeg(const float &verticalFovDeg);

    const float &getHorizontalFovDeg() const;
    void setHorizontalFovDeg(const float &horizontalFovDeg);

private:
    std::shared_ptr<core::TransformNode> transformNode_;
    float vertical_fov_deg_;
    float horizontal_fov_deg_;
    float vertical_fov_rad_;
    float horizontal_fov_rad_;
    std::string name_;
};
