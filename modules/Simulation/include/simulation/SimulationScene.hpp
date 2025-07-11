#pragma once

#include <geometry/implementations/Device.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <core/PointCloud.hpp>
#include <core/Car.hpp>
#include <core/Alias.hpp>

#include <vector>
#include <memory>

class SimulationScene
{
public:
    SimulationScene();

    // Add transmitters / receivers / shapes
    void addShape(std::shared_ptr<ShapeBase> shape);
    void setShapes(SharedVec<ShapeBase> shapes);

    // Accessors
    const std::shared_ptr<Car> &getCar() const;
    void setCar(std::shared_ptr<Car> car);

    const SharedVec<Device> &getTransmitters() const;
    const SharedVec<Device> &getReceivers() const;
    const SharedVec<ShapeBase> &getShapes() const;

    bool hasCar() const;

    // Merged point cloud from all shapes
    std::shared_ptr<PointCloud> getMergedPointCloud(int quality = 2048) const;

private:
    std::shared_ptr<Car> car_;
    SharedVec<ShapeBase> shapes_;
};
