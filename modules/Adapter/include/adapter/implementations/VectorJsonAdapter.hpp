#pragma once

#include <math/Vector.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{

    class VectorJsonAdapter : public IJsonAdapter<Vector>
    {
    public:
        nlohmann::json toJson(const Vector &vector) const override;
        Vector fromJson(const nlohmann::json &j) const override;
    };

} // namespace adapter
