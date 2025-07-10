#include <adapter/implementations/DeviceJsonAdapter.hpp>
#include <iostream>

namespace adapter
{

    DeviceJsonAdapter::DeviceJsonAdapter()
        : pointAdapter_(), vectorAdapter_()
    {
    }

    nlohmann::json DeviceJsonAdapter::toJson(const Device &device) const
    {
        nlohmann::json j;
        j["name"] = device.getName();
        j["origin"] = pointAdapter_.toJson(device.getOrigin());
        j["orientation"] = vectorAdapter_.toJson(device.getOrientation());
        j["vertical_fov_deg"] = device.getVerticalFovDeg();
        j["horizontal_fov_deg"] = device.getHorizontalFovDeg();
        return j;
    }

    Device DeviceJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point origin = pointAdapter_.fromJson(j.at("origin"));
        Vector v = vectorAdapter_.fromJson(j.at("orientation"));
        Vector v_rad(RotationUtils::deg2rad(v.x()), RotationUtils::deg2rad(v.y()), RotationUtils::deg2rad(v.z()));
        float verticalFov = j.at("vertical_fov_deg").get<float>();
        float horizontalFov = j.at("horizontal_fov_deg").get<float>();
        std::string name = j.value("name", ""); // Opsiyonel, yoksa boş string

        spatial::Transform t(origin, v_rad);
        DeviceConfig config{
            t,
            verticalFov,
            horizontalFov,
            name};

        return Device(config);
    }

} // namespace adapter
