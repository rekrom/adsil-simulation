#pragma once
#include <core/Alias.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <core/Car.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <adapter/implementations/DeviceJsonAdapter.hpp>
#include <nlohmann/json.hpp>

namespace adapter
{

    class CarJsonAdapter : public IJsonAdapter<std::shared_ptr<Car>>
    {
    public:
        CarJsonAdapter();

        nlohmann::json toJson(const std::shared_ptr<Car> &car) const override;
        std::shared_ptr<Car> fromJson(const nlohmann::json &j) const override;

    private:
        PointJsonAdapter pointAdapter_;
        VectorJsonAdapter vectorAdapter_;
        DeviceJsonAdapter deviceAdapter_; // Device adapter eklendi
    };

} // namespace adapter
