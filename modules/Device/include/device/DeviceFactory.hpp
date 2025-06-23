#pragma once

#include <device/Device.hpp>
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

    static std::vector<std::shared_ptr<Device>> createDevices(
        const std::string &namePrefix,
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        LogCallback onCreate = nullptr);

    // Wrappers for clarity
    static std::vector<std::shared_ptr<Device>> createTransmitters(
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        LogCallback onCreate = nullptr);

    static std::vector<std::shared_ptr<Device>> createReceivers(
        const std::vector<Point> &origins,
        const std::vector<Vector> &directions,
        const std::vector<float> &vertical_fovs,
        const std::vector<float> &horizontal_fovs,
        LogCallback onCreate = nullptr);
};
