#pragma once

#include <geometry/implementations/Device.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <core/PointCloud.hpp>
#include <core/Car.hpp>

#include <vector>
#include <memory>

class SimulationScene
{
public:
    SimulationScene();

    // Add transmitters / receivers / shapes
    void addShape(std::shared_ptr<ShapeBase> shape);

    // Accessors
    const std::shared_ptr<Car> &getCar() const;
    void setCar(std::shared_ptr<Car> car);

    const std::vector<std::shared_ptr<Device>> &getTransmitters() const;
    const std::vector<std::shared_ptr<Device>> &getReceivers() const;
    const std::vector<std::shared_ptr<ShapeBase>> &getShapes() const;

    bool hasCar() const;

    // Merged point cloud from all shapes
    std::shared_ptr<PointCloud> getMergedPointCloud(int quality = 25) const;

private:
    std::shared_ptr<Car> car_;
    std::vector<std::shared_ptr<ShapeBase>> shapes_;
};
