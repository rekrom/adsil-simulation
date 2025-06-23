#pragma once

#include <json/JsonSerializable.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <device/Device.hpp>
#include <vector>
#include <memory>
#include <string>

class Car : public JsonSerializable
{
public:
    Car();
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

    // JSON Serialization
    nlohmann::json toJson() const override;
    void fromJson(const nlohmann::json &j) override;

private:
    Point position_;
    Vector orientation_; // roll, pitch, yaw
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<Point> trajectory_;
};
