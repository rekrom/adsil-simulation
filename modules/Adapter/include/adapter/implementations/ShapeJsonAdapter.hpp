#pragma once

#include <adapter/interfaces/IJsonAdapter.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <memory>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <core/RotationUtils.hpp>

namespace adapter
{

    class ShapeJsonAdapter : public IJsonAdapter<std::shared_ptr<ShapeBase>>
    {
    public:
        nlohmann::json toJson(const std::shared_ptr<ShapeBase> &shape) const override;
        std::shared_ptr<ShapeBase> fromJson(const nlohmann::json &j) const override;

    private:
        PointJsonAdapter pointAdapter_;
        VectorJsonAdapter vectorAdapter_;
    };

}
