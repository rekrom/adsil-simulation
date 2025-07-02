#include <core/Car.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

Car::Car()
    : transmitters_(),
      receivers_(),
      trajectory_(),
      dimension(CarDimension(1, 1, 1))
{
    transformNode_ = std::make_shared<spatial::TransformNode>();
    trajectory_.push_back(transformNode_->getLocalTransform().getPosition());
}

Car::Car(const CarConfig &config)
    : transmitters_(config.transmitters),
      receivers_(config.receivers),
      trajectory_(),
      dimension(config.dimension)
{
    transformNode_ = config.transformNode;
    // Setup device transform nodes parented to car node
    for (auto &device : transmitters_)
    {
        if (!device->getTransformNode())
            device->setTransformNode(std::make_shared<spatial::TransformNode>());
        device->getTransformNode()->setParent(transformNode_);
    }
    for (auto &device : receivers_)
    {
        if (!device->getTransformNode())
            device->setTransformNode(std::make_shared<spatial::TransformNode>());
        device->getTransformNode()->setParent(transformNode_);
    }

    trajectory_.push_back(getPosition());
}

std::vector<std::shared_ptr<Device>> Car::getTransmitters() const
{
    return transmitters_;
}
std::vector<std::shared_ptr<Device>> Car::getReceivers() const
{
    return receivers_;
}

std::vector<std::shared_ptr<Device>> Car::getAllDevices() const
{
    std::vector<std::shared_ptr<Device>> all = transmitters_;
    all.insert(all.end(), receivers_.begin(), receivers_.end());
    return all;
}

void Car::moveTo(const Point &newPosition)
{
    auto localTransform = transformNode_->getLocalTransform();
    localTransform.setPosition(newPosition);
    transformNode_->setLocalTransform(localTransform);

    trajectory_.push_back(newPosition);
}

void Car::moveForward(float step)
{
    Vector orientation = getOrientation();
    float yaw = orientation.z();

    float dx = std::sin(yaw) * step;
    float dy = std::cos(yaw) * step;
    float dz = 0.0f;

    Point currentPos = getPosition();
    moveTo(Point(currentPos.x() + dx, currentPos.y() + dy, currentPos.z() + dz));
}

void Car::rotateYaw(float angleDeg)
{
    float angleRad = angleDeg * static_cast<float>(M_PI) / 180.0f;

    auto localTransform = transformNode_->getLocalTransform();
    Vector ori = localTransform.getOrientation();
    ori = ori + Vector(0.f, 0.f, angleRad); // Assuming operator+ is rotation addition
    localTransform.setOrientation(ori);

    transformNode_->setLocalTransform(localTransform);
}

Transform Car::getTransform() const
{
    return getTransformNode()->getGlobalTransform();
}
Point Car::getPosition() const
{
    return getTransformNode()->getGlobalTransform().getPosition();
}
Vector Car::getOrientation() const
{
    return getTransformNode()->getGlobalTransform().getOrientation();
}

CarDimension Car::getDimension() const
{
    return dimension;
}

std::shared_ptr<spatial::TransformNode> Car::getTransformNode() const
{
    return transformNode_;
}

// void Car::setTransformNode(std::shared_ptr<spatial::TransformNode> transformNode)
// {
//     transformNode_ = std::move(transformNode);
// }

const std::vector<Point> &Car::getTrajectory() const { return trajectory_; }

std::string Car::toString() const
{
    std::ostringstream oss;
    oss << "Car(pos=" << transformNode_->getLocalTransform().getPosition().toString()
        << ", rpy=" << transformNode_->getLocalTransform().getOrientation().toString() << ")";
    return oss.str();
}

Transform Car::getDeviceWorldTransform(const Device &device) const
{
    if (!device.getTransformNode())
    {
        // fallback: return device's local transform
        return device.getTransform();
    }
    return device.getTransformNode()->getGlobalTransform();
}