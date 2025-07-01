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
        // direction -> placement_angles dönüşümü:
        Vector direction = device.getDirection();
        std::cout << direction.toString() << std::endl;
        // dir = (x, y, z)
        // vertical_angle_deg = atan(z / y) * 180 / pi
        // horizontal_angle_deg = atan(x / y) * 180 / pi
        double horizontal_angle_deg = RotationUtils::rad2deg(atan2(direction.x(), direction.y()));
        double vertical_angle_deg = RotationUtils::rad2deg(asin(direction.z()));

        j["placement_angles"] = {
            {"horizontal_angle_deg", horizontal_angle_deg},
            {"vertical_angle_deg", vertical_angle_deg}};

        j["vertical_fov_deg"] = device.getVerticalFovDeg();
        j["horizontal_fov_deg"] = device.getHorizontalFovDeg();
        return j;
    }

    Device DeviceJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point origin = pointAdapter_.fromJson(j.at("origin"));
        adapter::placement_angle pa = j.at("placement_angles").get<adapter::placement_angle>();

        double hRad = RotationUtils::deg2rad(pa.horizontal_angle_deg);
        double vRad = RotationUtils::deg2rad(pa.vertical_angle_deg);

        float x = cos(vRad) * sin(hRad);
        float y = cos(vRad) * cos(hRad);
        float z = sin(vRad);

        Vector direction(x, y, z);
        direction = direction.normalized();

        Transform t(origin, direction);

        float verticalFov = j.at("vertical_fov_deg").get<float>();
        float horizontalFov = j.at("horizontal_fov_deg").get<float>();
        std::string name = j.value("name", ""); // Opsiyonel, yoksa boş string
        DeviceConfig config{
            t,
            verticalFov,
            horizontalFov,
            name};

        return Device(config);
    }

} // namespace adapter
