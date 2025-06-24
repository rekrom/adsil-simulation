#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace utils
{

    bool readJsonFromFile(const std::string &filename, nlohmann::json &j);
    bool writeJsonToFile(const std::string &filename, const nlohmann::json &j);

}
