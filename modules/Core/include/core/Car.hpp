#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <core/configs/CarConfig.hpp>
#include <spatial/implementations/HasTransformNodeBase.hpp>
#include <geometry/implementations/Device.hpp>
#include <vector>
#include <memory>
#include <string>
#include <spatial/implementations/Transform.hpp>

class Car : public spatial::HasTransformNodeBase
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

    std::shared_ptr<spatial::TransformNode> getTransformNode() const override;

    Transform getTransform() const;
    Point getPosition() const;
    Vector getOrientation() const;
    CarDimension getDimension() const;

private:
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;
    CarDimension dimension;

private:
    Transform getDeviceWorldTransform(const Device &device) const;
};
