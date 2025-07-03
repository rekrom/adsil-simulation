#pragma once

#include <adapter/interfaces/IJsonAdapter.hpp>
#include <geometry/implementations/Device.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <nlohmann/json.hpp>
#include <core/RotationUtils.hpp>

namespace adapter
{
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
