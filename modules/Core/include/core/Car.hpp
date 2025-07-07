#pragma once

#include <core/Alias.hpp>
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

    const SharedVec<Device> &getTransmitters() const;
    const SharedVec<Device> &getReceivers() const;
    SharedVec<Device> getAllDevices() const;

    const std::vector<Point> &getTrajectory() const;
    std::string toString() const;

    Point getPosition() const;
    Vector getOrientation() const;
    CarDimension getDimension() const;

    void setPosition(const Point &position);
    void setOrientation(const Vector &rpy); // roll, pitch, yaw in radians
    void setDimension(const CarDimension &dim);

    const std::string &getName() const { return name_; }

public:
    static constexpr CarDimension DefaultCarDimension{2.53f, 1.39f, 1.52f};

private:
    SharedVec<Device> transmitters_;
    SharedVec<Device> receivers_;
    std::vector<Point> trajectory_;
    CarDimension dimension;
    std::string name_;

private:
    Transform getDeviceWorldTransform(const Device &device) const;
};
