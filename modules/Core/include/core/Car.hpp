#pragma once

#include <core/Alias.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
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

    const SharedVec<Device> &getTransmitters() const;
    const SharedVec<Device> &getReceivers() const;
    SharedVec<Device> getAllDevices() const;

    const std::vector<math::Point> &getTrajectory() const;
    std::string toString() const;

    CarDimension getDimension() const;

    void setDimension(const CarDimension &dim);

    const std::string &getName() const { return name_; }

public:
    static constexpr CarDimension DefaultCarDimension{2.53F, 1.39F, 1.52F};

private:
    SharedVec<Device> transmitters_;
    SharedVec<Device> receivers_;
    std::vector<math::Point> trajectory_;
    CarDimension dimension_;
    std::string name_;
};
