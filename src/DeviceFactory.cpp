#include "DeviceFactory.hpp"
#include "DeviceConfig.hpp"

std::vector<std::shared_ptr<Device>> DeviceFactory::createDevices(
    const std::string &namePrefix,
    const std::vector<Point> &origins,
    const std::vector<Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    LogCallback onCreate)
{
    std::vector<std::shared_ptr<Device>> devices;

    for (std::size_t i = 0; i < directions.size(); ++i)
    {
        DeviceConfig config{
            origins[i],
            directions[i],
            vertical_fovs[i],
            horizontal_fovs[i],
            namePrefix + std::to_string(i + 1)};
        auto device = std::make_shared<Device>(config);

        if (onCreate)
        {
            onCreate(*device);
        }

        devices.push_back(device);
    }

    return devices;
}

std::vector<std::shared_ptr<Device>> DeviceFactory::createTransmitters(
    const std::vector<Point> &origins,
    const std::vector<Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    LogCallback onCreate)
{
    return createDevices("t", origins, directions, vertical_fovs, horizontal_fovs, onCreate);
}

std::vector<std::shared_ptr<Device>> DeviceFactory::createReceivers(
    const std::vector<Point> &origins,
    const std::vector<Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    LogCallback onCreate)
{
    return createDevices("r", origins, directions, vertical_fovs, horizontal_fovs, onCreate);
}
