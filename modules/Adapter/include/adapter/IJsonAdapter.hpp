#pragma once

#include <adapter/IJsonAdapterBase.hpp>
#include <nlohmann/json.hpp>
namespace adapter
{

    template <typename T>
    class IJsonAdapter : public IJsonAdapterBase
    {
    public:
        virtual ~IJsonAdapter() = default;
        virtual nlohmann::json toJson(const T &obj) const = 0;
        virtual T fromJson(const nlohmann::json &j) const = 0;
    };

}
