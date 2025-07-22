#pragma once

#include <math/Point.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
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
