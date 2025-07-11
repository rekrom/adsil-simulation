#pragma once

#include <geometry/implementations/Device.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <string>

class DeviceFactory
{
public:
    using LogCallback = std::function<void(const Device &)>;

    static SharedVec<Device> createDevices(
        const std::string &namePrefix,
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        const std::vector<float> &ranges,

        LogCallback onCreate = nullptr);

    // Wrappers for clarity
    static SharedVec<Device> createTransmitters(
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        const std::vector<float> &ranges,

        LogCallback onCreate = nullptr);

    static SharedVec<Device> createReceivers(
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        const std::vector<float> &ranges,

        LogCallback onCreate = nullptr);
};
