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

const SharedVec<Device> &Car::getTransmitters() const
{
    return transmitters_;
}
const SharedVec<Device> &Car::getReceivers() const
{
    return receivers_;
}

SharedVec<Device> Car::getAllDevices() const
{
    SharedVec<Device> all = transmitters_;
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

    // TODO: check with mustafa
    float pitch = orientation.x();
    float yaw = orientation.y();
    // float roll = orientation.z(); // not needed for forward

    float x = std::cos(pitch) * std::sin(yaw);
    float y = std::sin(pitch);
    float z = std::cos(pitch) * std::cos(yaw);

    Vector forward(x, y, z);
    forward = forward.normalized();

    // Scale by step size
    Vector displacement = forward * step;

    // Get current position and move
    Point currentPos = getPosition();
    moveTo(currentPos + displacement);
}

void Car::rotateYaw(float angleDeg)
{
    float angleRad = angleDeg * static_cast<float>(M_PI) / 180.0f;

    auto localTransform = transformNode_->getLocalTransform();
    Vector ori = localTransform.getOrientation();
    ori = ori + Vector(0.f, angleRad, 0.f); // Modify yaw (Y axis)
    localTransform.setOrientation(ori);

    transformNode_->setLocalTransform(localTransform);
}

Point Car::getPosition() const
{
    return getTransformNode()->getGlobalTransform().getPosition();
}

Vector Car::getOrientation() const
{
    return getTransformNode()->getGlobalTransform().getOrientation();
}
void Car::setPosition(const Point &position)
{
    Transform t = getTransformNode()->getLocalTransform();
    t.setPosition(position);
    getTransformNode()->setLocalTransform(t);
}

void Car::setOrientation(const Vector &rpy)
{
    Transform t = getTransformNode()->getLocalTransform();
    t.setOrientation(rpy); // assumes rpy = (roll, pitch, yaw)
    getTransformNode()->setLocalTransform(t);
}

CarDimension Car::getDimension() const
{
    return dimension;
}

void Car::setDimension(const CarDimension &newCarDimension)
{
    dimension = newCarDimension;
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
        return device.getTransformNode()->getLocalTransform();
    }
    return device.getTransformNode()->getGlobalTransform();
}