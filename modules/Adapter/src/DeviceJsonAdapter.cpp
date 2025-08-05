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
        const auto &transform = device.getTransformNode()->getLocalTransform();
        j["origin"] = pointAdapter_.toJson(transform.getPosition());

        // Convert orientation from radians to degrees for JSON storage
        const Vector &orientation_rad = transform.getOrientation();
        Vector orientation_deg(
            math::RotationUtils::rad2deg(orientation_rad.x()),
            math::RotationUtils::rad2deg(orientation_rad.y()),
            math::RotationUtils::rad2deg(orientation_rad.z()));
        j["orientation"] = vectorAdapter_.toJson(orientation_deg);

        j["vertical_fov_deg"] = device.getVerticalFovDeg();
        j["horizontal_fov_deg"] = device.getHorizontalFovDeg();
        j["range"] = device.getRange();

        return j;
    }

    Device DeviceJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point origin = pointAdapter_.fromJson(j.at("origin"));
        Vector v = vectorAdapter_.fromJson(j.at("orientation"));

        // Convert orientation from degrees (stored in JSON) to radians (used internally)
        Vector v_rad(
            math::RotationUtils::deg2rad(v.x()),
            math::RotationUtils::deg2rad(v.y()),
            math::RotationUtils::deg2rad(v.z()));

        float verticalFov = j.at("vertical_fov_deg").get<float>();
        float horizontalFov = j.at("horizontal_fov_deg").get<float>();
        float range = j.value("range", 2.0f);
        std::string name = j.value("name", ""); // Optional, defaults to empty string

        spatial::Transform t(origin, v_rad);
        DeviceConfig config{
            t,
            verticalFov,
            horizontalFov,
            range,
            name};

        return Device(config);
    }

} // namespace adapter
