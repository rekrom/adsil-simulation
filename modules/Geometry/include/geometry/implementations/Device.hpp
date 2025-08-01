#pragma once

#include <core/Alias.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <string>
#include <memory>
#include <geometry/configs/DeviceConfig.hpp>
#include <spatial/implementations/HasTransform.hpp>
#include <math/math.hpp>
class Device : public spatial::HasTransform
{
public:
    Device(const DeviceConfig &config);

    std::shared_ptr<math::PointCloud> pointsInFov(const math::PointCloud &pcd) const;

    std::string toString() const;

    const std::string &getName() const { return name_; }

    const float &getRange() const;
    void setRange(float newRange);

    // Degrees
    float getVerticalFovDeg() const;
    void setVerticalFovDeg(float verticalFovDeg);
    float getHorizontalFovDeg() const;
    void setHorizontalFovDeg(float horizontalFovDeg);

    // Radians
    float getHorizontalFovRad() const;
    void setHorizontalFovRad(float horizontalFovRad);
    float getVerticalFovRad() const;
    void setVerticalFovRad(float verticalFovRad);

private:
    float vertical_fov_rad_;
    float horizontal_fov_rad_;
    float range_;
    std::string name_;
};
