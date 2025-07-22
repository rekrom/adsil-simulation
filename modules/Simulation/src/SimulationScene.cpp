#include <simulation/SimulationScene.hpp>

SimulationScene::SimulationScene()
    : car_(nullptr),
      shapes_(),
      externalCloud_(nullptr),
      timestamp_(0.0)
{
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
std::shared_ptr<PointCloud> SimulationScene::getMergedPointCloud(int quality) const
{
    if (externalCloud_)
    {
        return externalCloud_;
    }
    return mergedShapePointCloud(quality);
}

std::shared_ptr<PointCloud> SimulationScene::mergedShapePointCloud(int quality) const
{
    auto merged = std::make_shared<PointCloud>();

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

void SimulationScene::setExternalPointCloud(std::shared_ptr<PointCloud> cloud)
{
    externalCloud_ = std::move(cloud);
}

std::shared_ptr<PointCloud> SimulationScene::getExternalPointCloud() const
{
    return externalCloud_;
}

void SimulationScene::onFrameChanged(const std::shared_ptr<simulation::Frame> &frame)
{
    if (frame)
    {
        overrideTimestamp(frame->timestamp);
        setExternalPointCloud(frame->cloud);
        // LOGGER_INFO("SimulationScene: Frame changed, timestamp updated to " + std::to_string(frame->timestamp));
        if (this->getCar())
        {
            // LOGGER_INFO("SimulationScene: Updating car position and orientation based on frame data");

            if (!frame || frame->linearAcceleration.size() != 3 || frame->angularVelocity.size() != 3)
            {
                return;
            }
            // Update car's IMU data if available
            const auto &acc = frame->linearAcceleration;
            const auto &angVel = frame->angularVelocity;

            float frameInterval = 0.1f; // or inject this from SimulationManager

            Vector acceleration(acc[0] * 100, acc[1] * 100, acc[2] * 100);
            Vector angularVelocity(angVel[0], angVel[1], angVel[2]);

            Vector carVelocity_;
            carVelocity_ += acceleration * frameInterval;
            Vector displacement = carVelocity_ * frameInterval;
            this->getCar()->moveBy(displacement);

            Vector deltaEuler = angularVelocity * frameInterval;
            Vector carOrientation_;
            carOrientation_ += deltaEuler;
            this->getCar()->rotateByYawPitchRoll(carOrientation_.x(), carOrientation_.y(), carOrientation_.z());
        }
    }
}