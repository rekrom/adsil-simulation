#include <core/Car.hpp>
#include <cmath>
#include <sstream>

Car::Car()
    : position_(),
      orientation_(),
      transmitters_(),
      receivers_()
{
    trajectory_.push_back(position_);
}

Car::Car(const Point &position,
         const Vector &orientation,
         const std::vector<std::shared_ptr<Device>> &transmitters,
         const std::vector<std::shared_ptr<Device>> &receivers)
    : position_(position),
      orientation_(orientation),
      transmitters_(transmitters),
      receivers_(receivers)
{
    trajectory_.push_back(position_);
}

std::vector<std::shared_ptr<Device>> Car::getAllDevices() const
{
    std::vector<std::shared_ptr<Device>> all = transmitters_;
    all.insert(all.end(), receivers_.begin(), receivers_.end());
    return all;
}

void Car::moveTo(const Point &newPosition)
{
    Vector delta(newPosition.x() - position_.x(),
                 newPosition.y() - position_.y(),
                 newPosition.z() - position_.z());

    position_ = newPosition;

    for (auto &device : getAllDevices())
    {
        Point updatedOrigin = device->getOrigin() + delta;
        device->setOrigin(updatedOrigin);
    }

    trajectory_.push_back(newPosition);
}

void Car::moveForward(float step)
{
    float yaw = orientation_.z();
    float dx = std::sin(yaw) * step;
    float dy = std::cos(yaw) * step;
    float dz = 0.0f;

    moveTo(Point(position_.x() + dx, position_.y() + dy, position_.z() + dz));
}

void Car::rotateYaw(float angleDeg)
{
    float angleRad = angleDeg * static_cast<float>(M_PI) / 180.0f;
    float cosA = std::cos(angleRad);
    float sinA = std::sin(angleRad);

    auto rotate = [cosA, sinA](const Vector &v) -> Vector
    {
        return Vector(
            v.x() * cosA - v.y() * sinA,
            v.x() * sinA + v.y() * cosA,
            v.z());
    };

    for (auto &device : getAllDevices())
    {
        Vector rotatedDir = rotate(device->getDirection());
        device->setDirection(rotatedDir.normalized());
    }

    // update internal yaw
    orientation_ = Vector(orientation_.x(), orientation_.y(), orientation_.z() + angleRad);
}

const Point &Car::getPosition() const { return position_; }
const Vector &Car::getOrientation() const { return orientation_; }
const std::vector<Point> &Car::getTrajectory() const { return trajectory_; }

std::string Car::toString() const
{
    std::ostringstream oss;
    oss << "Car(pos=" << position_.toString()
        << ", rpy=" << orientation_.toString() << ")";
    return oss.str();
}

nlohmann::json Car::toJson() const
{
    return {
        {"position", {{"x", position_.x()}, {"y", position_.y()}, {"z", position_.z()}}}};
}

void Car::fromJson(const nlohmann::json &j)
{
    // position_.setX(j.at("position").at("x").get<float>());
    // position_.setY(j.at("position").at("y").get<float>());
    // position_.setZ(j.at("position").at("z").get<float>());
}