#pragma once

#include <math/Vector.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{

    class VectorJsonAdapter : public IJsonAdapter<math::Vector>
    {
    public:
        nlohmann::json toJson(const math::Vector &vector) const override;
        math::Vector fromJson(const nlohmann::json &j) const override;
    };

} // namespace adapter
