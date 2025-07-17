#pragma once

#include <core/Alias.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <core/Car.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <adapter/implementations/DeviceJsonAdapter.hpp>
#include <nlohmann/json.hpp>
#include <simulation/implementations/Frame.hpp>

namespace adapter
{
    class FrameJsonAdapter : public IJsonAdapter<std::shared_ptr<simulation::Frame>>
    {
    public:
        FrameJsonAdapter();

        nlohmann::json toJson(const std::shared_ptr<simulation::Frame> &car) const override;
        std::shared_ptr<simulation::Frame> fromJson(const nlohmann::json &j) const override;
    };
}
