#pragma once

#include <adapter/interfaces/IJsonAdapter.hpp>
#include <core/Car.hpp>
#include <adapter/implementations/implementations.hpp>
#include <nlohmann/json.hpp>
#include <simulation/SimulationScene.hpp>

namespace adapter
{
    class SceneJsonAdapter : public IJsonAdapter<std::shared_ptr<SimulationScene>>
    {
    public:
        SceneJsonAdapter();

        nlohmann::json toJson(const std::shared_ptr<SimulationScene> &scene) const override;
        std::shared_ptr<SimulationScene> fromJson(const nlohmann::json &j) const override;

    private:
        CarJsonAdapter carAdapter_;
        ShapeJsonAdapter shapeAdapter_;
        PointJsonAdapter pointAdapter_;
        // add others later (device, trajectory, etc.)
    };
}
