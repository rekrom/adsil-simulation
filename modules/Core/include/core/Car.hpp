#pragma once

#include <core/Alias.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <core/configs/CarConfig.hpp>
#include <spatial/implementations/implementations.hpp>
#include <geometry/implementations/Device.hpp>
#include <vector>
#include <memory>
#include <string>
#include <spatial/implementations/Transform.hpp>

class Car : public spatial::HasMovable
{
public:
    Car();

    Car(const CarConfig &config);

    void moveTo(const Point &newPosition);
    // void moveForward(float step = 100.0F);
    // void rotateYaw(float angleDeg);

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
    static constexpr CarDimension DefaultCarDimension{2.53F, 1.39F, 1.52F};

private:
    SharedVec<Device> transmitters_;
    SharedVec<Device> receivers_;
    std::vector<Point> trajectory_;
    CarDimension dimension;
    std::string name_;

private:
    Transform getDeviceWorldTransform(const Device &device) const;
};
