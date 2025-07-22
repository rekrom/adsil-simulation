#include <geometry/factories/DeviceFactory.hpp>
#include <geometry/configs/DeviceConfig.hpp>

SharedVec<Device> DeviceFactory::createDevices(
    const std::string &namePrefix,
    const std::vector<math::Point> &origins,
    const std::vector<math::Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    const std::vector<float> &ranges,
    LogCallback onCreate)
{
    SharedVec<Device> devices;

    for (std::size_t i = 0; i < directions.size(); ++i)
    {
        spatial::Transform t(origins[i], directions[i]);
        DeviceConfig config{
            t,
            vertical_fovs[i],
            horizontal_fovs[i],
            ranges[i],
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

SharedVec<Device> DeviceFactory::createTransmitters(
    const std::vector<math::Point> &origins,
    const std::vector<math::Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    const std::vector<float> &ranges,

    LogCallback onCreate)
{
    return createDevices("t", origins, directions, vertical_fovs, horizontal_fovs, ranges, onCreate);
}

SharedVec<Device> DeviceFactory::createReceivers(
    const std::vector<math::Point> &origins,
    const std::vector<math::Vector> &directions,
    const std::vector<float> &vertical_fovs,
    const std::vector<float> &horizontal_fovs,
    const std::vector<float> &ranges,

    LogCallback onCreate)
{
    return createDevices("r", origins, directions, vertical_fovs, horizontal_fovs, ranges, onCreate);
}
