#pragma once

#include <core/Point.hpp>
#include <adapter/IJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{

    class PointJsonAdapter : public IJsonAdapter<Point>
    {
    public:
        nlohmann::json toJson(const Point &point) const override;
        Point fromJson(const nlohmann::json &j) const override;
    };

} // namespace adapter
