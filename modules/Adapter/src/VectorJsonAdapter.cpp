#include <adapter/implementations/VectorJsonAdapter.hpp>

namespace adapter
{

    nlohmann::json VectorJsonAdapter::toJson(const math::Vector &vector) const
    {
        return {
            {"x", vector.x()},
            {"y", vector.y()},
            {"z", vector.z()}};
    }

    math::Vector VectorJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        return math::Vector(
            j.at("x").get<float>(),
            j.at("y").get<float>(),
            j.at("z").get<float>());
    }

} // namespace adapter
