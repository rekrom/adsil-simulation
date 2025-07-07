#pragma once

#include <core/Alias.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <core/RotationUtils.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <core/PointCloud.hpp>
#include <string>
#include <memory>
#include <geometry/configs/DeviceConfig.hpp>
#include <spatial/implementations/HasTransformNodeBase.hpp>

class Device : public spatial::HasTransformNodeBase
{
public:
    Device(const DeviceConfig &config);

    std::shared_ptr<PointCloud> pointsInFov(const PointCloud &pcd) const;

    std::string toString() const;

    const std::string &getName() const { return name_; }

    const Point &getOrigin() const;
    void setOrigin(const Point &newOrigin);

    const Vector &getOrientation() const;
    void setOrientation(const Vector &newOrientation);

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
