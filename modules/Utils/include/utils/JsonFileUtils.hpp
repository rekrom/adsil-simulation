#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

namespace utils
{

    bool readJsonFromFile(const std::string &filename, nlohmann::json &j);
    bool writeJsonToFile(const std::string &filename, const nlohmann::json &j);
    inline nlohmann::json loadJsonOrExit(const std::string &path)
    {
        nlohmann::json j;
        if (!readJsonFromFile(path, j))
        {
            std::cerr << "[ERROR] Failed to read JSON file: " << path << std::endl;
            std::exit(1);
        }
        return j;
    }

}
