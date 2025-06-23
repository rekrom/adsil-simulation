#include <json/JsonUtils.hpp>
#include <fstream>

namespace JsonUtils
{
    bool loadFromFile(const std::string &path, nlohmann::json &outJson)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        try
        {
            file >> outJson;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool saveToFile(const std::string &path, const nlohmann::json &json)
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        try
        {
            file << json.dump(4); // Pretty-print
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
}
