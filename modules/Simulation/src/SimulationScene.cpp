#include <simulation/SimulationScene.hpp>

SimulationScene::SimulationScene() = default;

void SimulationScene::addShape(std::shared_ptr<ShapeBase> shape)
{
    shapes_.push_back(std::move(shape));
}

void SimulationScene::setShapes(SharedVec<ShapeBase> shapes)
{
    shapes_ = shapes;
}

void SimulationScene::setCar(std::shared_ptr<Car> car)
{
    car_ = std::move(car);
}

const std::shared_ptr<Car> &SimulationScene::getCar() const
{
    return car_;
}

const SharedVec<Device> &SimulationScene::getTransmitters() const
{
    assert(car_ && "car_ must be set before accessing transmitters");
    return car_->getTransmitters();
}

const SharedVec<Device> &SimulationScene::getReceivers() const
{
    assert(car_ && "car_ must be set before accessing receivers");
    return car_->getReceivers();
}

const SharedVec<ShapeBase> &SimulationScene::getShapes() const
{
    return shapes_;
}

bool SimulationScene::hasCar() const
{
    return static_cast<bool>(car_);
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
