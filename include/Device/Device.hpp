#pragma once

#include "Core/Point.hpp"
#include "Core/Vector.hpp"
#include "Core/PointCloud.hpp"
#include <string>
#include <memory>
#include "Device/DeviceConfig.hpp"

class Device
{
public:
    Device(const DeviceConfig &config);

    std::shared_ptr<PointCloud> pointsInFov(const PointCloud &pcd) const;

    std::string toString() const;

    const std::string &getName() const { return name_; }

    const Point &getOrigin() const;
    void setOrigin(const Point &newOrigin);

    const Vector &getDirection() const;
    void setDirection(const Vector &newDirection);

private:
    Point origin_;
    Vector direction_;
    float vertical_fov_deg_;
    float horizontal_fov_deg_;
    float vertical_fov_rad_;
    float horizontal_fov_rad_;
    std::string name_;
};
