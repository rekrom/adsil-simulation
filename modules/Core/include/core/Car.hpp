#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <core/TransformNode.hpp>
#include <core/configs/CarConfig.hpp>
#include <core/HasTransformNodeBase.hpp>
#include <geometry/implementations/Device.hpp>
#include <vector>
#include <memory>
#include <string>
#include <core/Transform.hpp>

class Car : public core::HasTransformNodeBase
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

    const std::vector<Point> &getTrajectory() const;
    std::string toString() const;

    std::shared_ptr<core::TransformNode> getTransformNode() const override;

    Transform getTransform() const;
    Point getPosition() const;
    Vector getOrientation() const;

private:
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;

private:
    Transform getDeviceWorldTransform(const Device &device) const;
};
