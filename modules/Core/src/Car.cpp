#include <core/Car.hpp>
#include <cmath>
#include <sstream>
#include <iostream>

Car::Car()
    : transmitters_(),
      receivers_(),
      trajectory_(),
      dimension_(CarDimension(1, 1, 1)),
      name_("car")
{
    transformNode_ = std::make_shared<spatial::TransformNode>();
    trajectory_.push_back(transformNode_->getGlobalTransform().getPosition());
}

Car::Car(const CarConfig &config)
    : transmitters_(config.transmitters),
      receivers_(config.receivers),
      trajectory_(),
      dimension_(config.dimension),
      name_("car")
{
    transformNode_ = config.transformNode;
    // Setup device transform nodes parented to car node
    for (auto &device : getAllDevices())
    {
        if (!device->getTransformNode())
            device->setTransformNode(std::make_shared<spatial::TransformNode>());
        device->getTransformNode()->setParent(transformNode_);
    }

    trajectory_.push_back(transformNode_->getGlobalTransform().getPosition());
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
    auto localTransform = transformNode_->getGlobalTransform();
    localTransform.setPosition(newPosition);
    transformNode_->setLocalTransform(localTransform);

    trajectory_.push_back(newPosition);
}

CarDimension Car::getDimension() const
{
    return dimension_;
}

void Car::setDimension(const CarDimension &newCarDimension)
{
    dimension_ = newCarDimension;
}

const std::vector<math::Point> &Car::getTrajectory() const { return trajectory_; }

std::string Car::toString() const
{
    std::ostringstream oss;
    oss << "Car(pos=" << transformNode_->getGlobalTransform().getPosition().toString()
        << ", rpy=" << transformNode_->getGlobalTransform().getOrientation().toString() << ")";
    return oss.str();
}