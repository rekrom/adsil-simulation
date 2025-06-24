#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <device/Device.hpp>
#include <vector>
#include <memory>
#include <string>
#include <core/Transform.hpp>

class Car
{
public:
    Car();
    Car(const Transform &transform,
        const std::vector<std::shared_ptr<Device>> &transmitters,
        const std::vector<std::shared_ptr<Device>> &receivers);

    Car(const Point &position,
        const Vector &orientation,
        const std::vector<std::shared_ptr<Device>> &transmitters,
        const std::vector<std::shared_ptr<Device>> &receivers);

    void moveTo(const Point &newPosition);
    void moveForward(float step = 100.0f);
    void rotateYaw(float angleDeg);

    std::vector<std::shared_ptr<Device>> getTransmitters() const;
    std::vector<std::shared_ptr<Device>> getReceivers() const;
    std::vector<std::shared_ptr<Device>> getAllDevices() const;

    const Point &getPosition() const;
    const Vector &getOrientation() const;
    const Transform &getTransform() const;

    void setTransform(const Transform &transform);

    const std::vector<Point> &getTrajectory() const;

    std::string toString() const;

private:
    Point position_;
    Vector orientation_; // roll, pitch, yaw
    Transform transform_;
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;
};
