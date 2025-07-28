#include <simulation/SimulationScene.hpp>

SimulationScene::SimulationScene()
    : car_(nullptr),
      shapes_(),
      externalCloud_(nullptr),
      timestamp_(0.0)
{
    observerName_ = "SimulationSceneObserver";
}

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
std::shared_ptr<math::PointCloud> SimulationScene::getMergedPointCloud(int quality) const
{
    if (externalCloud_)
    {
        return externalCloud_;
    }
    return mergedShapePointCloud(quality);
}

std::shared_ptr<math::PointCloud> SimulationScene::mergedShapePointCloud(int quality) const
{
    auto merged = std::make_shared<math::PointCloud>();

    for (const auto &shape : shapes_)
    {
        if (!shape)
            continue;
        merged = shape->getSurfaceMeshPCD();
    }

    return merged;
}

double SimulationScene::getTimestamp() const
{
    return timestamp_;
}

void SimulationScene::overrideTimestamp(double ts)
{
    timestamp_ = ts;
}

void SimulationScene::setExternalPointCloud(std::shared_ptr<math::PointCloud> cloud)
{
    externalCloud_ = std::move(cloud);
}

std::shared_ptr<math::PointCloud> SimulationScene::getExternalPointCloud() const
{
    return externalCloud_;
}

void SimulationScene::onFrameChanged(const std::shared_ptr<simulation::Frame> &frame)
{
    LOGGER_INFO("SimulationScene received frame change notification");
    if (!frame)
    {
        LOGGER_WARN("SimulationScene: Received empty frame");
        return;
    }
    // setExternalPointCloud(frame->cloud);
    // overrideTimestamp(frame->timestamp);
}