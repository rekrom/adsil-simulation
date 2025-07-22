#pragma once

#include <math/Point.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{

    class PointJsonAdapter : public IJsonAdapter<math::Point>
    {
    public:
        nlohmann::json toJson(const math::Point &point) const override;
        math::Point fromJson(const nlohmann::json &j) const override;
    };

} // namespace adapter
