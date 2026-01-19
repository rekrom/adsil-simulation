#include <simulation/SimulationScene.hpp>

SimulationScene::SimulationScene()
    : car_(nullptr),
      shapes_(),
      externalCloud_(std::make_shared<math::PointCloud>()),
      timestamp_(0.0)
{
    observerName_ = "SimulationSceneObserver";
}

void SimulationScene::addShape(std::shared_ptr<ShapeBase> shape)
{
    shapes_.push_back(std::move(shape));
    // Invalidate cache when scene content changes
    mergedCacheDirty_ = true;
}

void SimulationScene::setShapes(SharedVec<ShapeBase> shapes)
{
    shapes_ = shapes;
    mergedCacheDirty_ = true;
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
    // Prefer external cloud only if it has data; otherwise synthesize from shapes
    if (externalCloud_ && !externalCloud_->empty())
    {
        return externalCloud_;
    }
    return mergedShapePointCloud(quality);
}

std::shared_ptr<math::PointCloud> SimulationScene::mergedShapePointCloud(int quality) const
{
    // Simple cache to avoid re-generating merged cloud every call if shapes/quality unchanged
    if (!mergedCacheDirty_ && mergedCache_ && lastQuality_ == quality)
    {
        LOGGER_INFO("Using cached merged point cloud");
        return mergedCache_;
    }

    auto merged = std::make_shared<math::PointCloud>();
    for (const auto &shape : shapes_)
    {
        if (!shape)
            continue;
        auto s = shape->getSurfaceMeshPCD();
        if (s && !s->empty())
        {
            merged->addPoints(s->getPoints());
        }
    }

    mergedCache_ = merged;
    lastQuality_ = quality;
    mergedCacheDirty_ = false;
    return mergedCache_;
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
    *externalCloud_ = *cloud; // ✅ overwrite contents, not pointer
    // External stream takes precedence; cache isn't useful while external data exists
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
    LOGGER_INFO("External point cloud updated with frame data");
    setExternalPointCloud(frame->cloud);
    // overrideTimestamp(frame->timestamp);
}