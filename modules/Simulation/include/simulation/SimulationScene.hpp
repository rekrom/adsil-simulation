#pragma once

#include <geometry/implementations/Device.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <core/PointCloud.hpp>
#include <core/Car.hpp>
#include <core/Alias.hpp>
#include <core/Logger.hpp>

#include <simulation/interfaces/ISimulationScene.hpp>
#include <simulation/interfaces/IFrameObserver.hpp>
#include <simulation/implementations/Frame.hpp>

#include <vector>
#include <memory>

class SimulationScene : public ISimulationScene, public simulation::IFrameObserver
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
    std::shared_ptr<PointCloud> getMergedPointCloud(int quality = 2048) const override;

    // Timestamp override
    double getTimestamp() const override;

    // Inject real data
    void overrideTimestamp(double ts);
    void setExternalPointCloud(std::shared_ptr<PointCloud> cloud);
    std::shared_ptr<PointCloud> getExternalPointCloud() const;

    void onFrameChanged(const std::shared_ptr<simulation::Frame> &frame) override;

private:
    std::shared_ptr<Car> car_;
    SharedVec<ShapeBase> shapes_;

    // Real-frame override
    std::shared_ptr<PointCloud> externalCloud_ = nullptr;
    double timestamp_ = 0.0;

    // Internal helper
    std::shared_ptr<PointCloud> mergedShapePointCloud(int quality) const;
};
