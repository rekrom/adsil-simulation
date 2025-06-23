#include <simulation/SimulationScene.hpp>

SimulationScene::SimulationScene() = default;

void SimulationScene::addTransmitter(std::shared_ptr<Device> tx)
{
    transmitters_.push_back(std::move(tx));
}

void SimulationScene::addTransmitters(const std::vector<std::shared_ptr<Device>> &devices)
{
    transmitters_.insert(transmitters_.end(), devices.begin(), devices.end());
}

void SimulationScene::addReceiver(std::shared_ptr<Device> rx)
{
    receivers_.push_back(std::move(rx));
}

void SimulationScene::addReceivers(const std::vector<std::shared_ptr<Device>> &devices)
{
    receivers_.insert(receivers_.end(), devices.begin(), devices.end());
}

void SimulationScene::addShape(std::shared_ptr<IShape> shape)
{
    shapes_.push_back(std::move(shape));
}

const std::vector<std::shared_ptr<Device>> &SimulationScene::getTransmitters() const
{
    return transmitters_;
}

const std::vector<std::shared_ptr<Device>> &SimulationScene::getReceivers() const
{
    return receivers_;
}

const std::vector<std::shared_ptr<IShape>> &SimulationScene::getShapes() const
{
    return shapes_;
}

std::shared_ptr<PointCloud> SimulationScene::getMergedPointCloud(int quality) const
{
    auto merged = std::make_shared<PointCloud>();
    for (const auto &shape : shapes_)
    {
        auto cloud = shape->surfaceMesh(quality);
        merged->addPoints(cloud->getPoints());
    }
    return merged;
}
