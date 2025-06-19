#pragma once

#include "Core/Point.hpp"
#include "Core/Vector.hpp"
#include "Device/Device.hpp"
#include <vector>
#include <memory>
#include <string>

class Car
{
public:
    Car(const Point &position,
        const Vector &orientation,
        const std::vector<std::shared_ptr<Device>> &transmitters,
        const std::vector<std::shared_ptr<Device>> &receivers);

    void moveTo(const Point &newPosition);
    void moveForward(float step = 100.0f);
    void rotateYaw(float angleDeg);

    std::vector<std::shared_ptr<Device>> getAllDevices() const;

    const Point &getPosition() const;
    const Vector &getOrientation() const;
    const std::vector<Point> &getTrajectory() const;

    std::string toString() const;

private:
    Point position_;
    Vector orientation_; // roll, pitch, yaw
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;
};
