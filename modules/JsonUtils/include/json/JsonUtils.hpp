#pragma once
#include <nlohmann/json.hpp>
#include <string>

namespace JsonUtils
{
    // JSON dosyasından veri okuma
    bool loadFromFile(const std::string &path, nlohmann::json &outJson);

    // JSON verisini dosyaya yazma
    bool saveToFile(const std::string &path, const nlohmann::json &json);
}
