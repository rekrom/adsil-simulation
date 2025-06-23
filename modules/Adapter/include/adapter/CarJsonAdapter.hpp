#pragma once

#include <adapter/IJsonAdapter.hpp>
#include <core/Car.hpp>
#include <adapter/PointJsonAdapter.hpp>
#include <adapter/VectorJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{
    class CarJsonAdapter : public adapter::IJsonAdapter<Car>
    {
    public:
        CarJsonAdapter();

        nlohmann::json toJson(const Car &car) const override;
        Car fromJson(const nlohmann::json &j) const override;

    private:
        adapter::PointJsonAdapter pointAdapter_;
        adapter::VectorJsonAdapter vectorAdapter_;
        // TODO: Device adapter gerekirse buraya ekle
    };
}
