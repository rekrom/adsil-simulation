#pragma once

#include <adapter/interfaces/IJsonAdapter.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <memory>

namespace adapter
{

    class ShapeJsonAdapter : public IJsonAdapter<std::shared_ptr<IShape>>
    {
    public:
        nlohmann::json toJson(const std::shared_ptr<IShape> &shape) const override;
        std::shared_ptr<IShape> fromJson(const nlohmann::json &j) const override;
    };

}
