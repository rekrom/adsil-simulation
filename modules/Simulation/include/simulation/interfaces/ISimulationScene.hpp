#pragma once

#include <math/PointCloud.hpp>

class ISimulationScene
{
public:
    virtual ~ISimulationScene() = default;

    // Main data source for the simulation
    virtual std::shared_ptr<PointCloud> getMergedPointCloud(int quality = 2048) const = 0;

    // Time tagging (used for timeline, logging, etc.)
    virtual double getTimestamp() const = 0;
};
