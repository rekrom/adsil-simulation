#pragma once

#include <adapter/interfaces/IJsonAdapter.hpp>
#include <geometry/implementations/Device.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <nlohmann/json.hpp>
#include <core/RotationUtils.hpp>

namespace adapter
{
    struct placement_angle
    {
        float horizontal_angle_deg;
        float vertical_angle_deg;
    };
    inline void from_json(const nlohmann::json &j, placement_angle &pa)
    {
        j.at("horizontal_angle_deg").get_to(pa.horizontal_angle_deg);
        j.at("vertical_angle_deg").get_to(pa.vertical_angle_deg);
    }

    inline void to_json(nlohmann::json &j, const placement_angle &pa)
    {
        j = nlohmann::json{
            {"horizontal_angle_deg", pa.horizontal_angle_deg},
            {"vertical_angle_deg", pa.vertical_angle_deg}};
    }

    class DeviceJsonAdapter : public IJsonAdapter<Device>
    {
    public:
        DeviceJsonAdapter();

        nlohmann::json toJson(const Device &device) const override;
        Device fromJson(const nlohmann::json &j) const override;

    private:
        PointJsonAdapter pointAdapter_;
        VectorJsonAdapter vectorAdapter_;
    };

} // namespace adapter
