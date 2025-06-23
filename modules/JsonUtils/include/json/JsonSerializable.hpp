#pragma once
#include <nlohmann/json.hpp>

class JsonSerializable
{
public:
    virtual ~JsonSerializable() = default;

    virtual nlohmann::json toJson() const = 0;
    virtual void fromJson(const nlohmann::json &j) = 0;
};
