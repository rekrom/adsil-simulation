#pragma once

#include <device/Device.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <core/PointCloud.hpp>

#include <vector>
#include <memory>

class SimulationScene
{
public:
    SimulationScene();

    // Add transmitters / receivers / shapes
    void addTransmitter(std::shared_ptr<Device> tx);
    void addTransmitters(const std::vector<std::shared_ptr<Device>> &devices);

    void addReceiver(std::shared_ptr<Device> rx);
    void addReceivers(const std::vector<std::shared_ptr<Device>> &devices);

    void addShape(std::shared_ptr<IShape> shape);

    // Accessors
    const std::vector<std::shared_ptr<Device>> &getTransmitters() const;
    const std::vector<std::shared_ptr<Device>> &getReceivers() const;
    const std::vector<std::shared_ptr<IShape>> &getShapes() const;

    // Merged point cloud from all shapes
    std::shared_ptr<PointCloud> getMergedPointCloud(int quality = 25) const;

private:
    std::vector<std::shared_ptr<Device>> transmitters_;
    std::vector<std::shared_ptr<Device>> receivers_;
    std::vector<std::shared_ptr<IShape>> shapes_;
};
