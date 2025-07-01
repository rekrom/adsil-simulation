#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <core/TransformNode.hpp>
#include <core/configs/CarConfig.hpp>
#include <device/Device.hpp>
#include <vector>
#include <memory>
#include <string>
#include <core/Transform.hpp>

class Car
{
public:
    Car();

    Car(const CarConfig &config);

    void moveTo(const Point &newPosition);
    void moveForward(float step = 100.0f);
    void rotateYaw(float angleDeg);

    std::vector<std::shared_ptr<Device>> getTransmitters() const;
    std::vector<std::shared_ptr<Device>> getReceivers() const;
    std::vector<std::shared_ptr<Device>> getAllDevices() const;

    const Point &getPosition() const;
    const Vector &getOrientation() const;
    const Transform &getTransform() const;
    std::shared_ptr<core::TransformNode> getTransformNode() const;
    Transform getGlobalTransform();

    void setTransformNode(std::shared_ptr<core::TransformNode> transformNode);

    const std::vector<Point> &getTrajectory() const;

    std::string toString() const;

private:
    // Transform transform_;
    std::shared_ptr<core::TransformNode> transformNode_;
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;

private:
    Transform getDeviceWorldTransform(const Device &device) const;
};
