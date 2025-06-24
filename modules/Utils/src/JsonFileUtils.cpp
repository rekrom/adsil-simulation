#include "utils/JsonFileUtils.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace utils
{

    bool readJsonFromFile(const std::string &filename, nlohmann::json &j)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Dosya açılamadı: " << filename << std::endl;
            return false;
        }
        try
        {
            file >> j;
        }
        catch (const std::exception &e)
        {
            std::cerr << "JSON parse hatası: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool writeJsonToFile(const std::string &filename, const nlohmann::json &j)
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Dosya açılamadı: " << filename << std::endl;
            return false;
        }
        try
        {
            file << j.dump(4);
        }
        catch (const std::exception &e)
        {
            std::cerr << "JSON yazma hatası: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

} // namespace utils
