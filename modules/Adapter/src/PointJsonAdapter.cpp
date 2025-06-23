#include <adapter/PointJsonAdapter.hpp>

namespace adapter
{

    nlohmann::json PointJsonAdapter::toJson(const Point &point) const
    {
        return {
            {"x", point.x()},
            {"y", point.y()},
            {"z", point.z()}};
    }

    Point PointJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        return Point(
            j.at("x").get<float>(),
            j.at("y").get<float>(),
            j.at("z").get<float>());
    }

} // namespace adapter
