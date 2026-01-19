#include <simulation/configs/SimulationConfig.hpp>
#include <stdexcept>
#include <cstdlib>

namespace simulation
{
    std::shared_ptr<SimulationConfig> SimulationConfig::createDefault()
    {
        auto config = std::make_shared<SimulationConfig>();

        // Override base path from environment variable if available
        const char *basePathEnv = std::getenv("ADSIL_RESOURCE_PATH");
        if (basePathEnv)
        {
            ResourceConfig resourceConfig = config->getResourceConfig();
            resourceConfig.basePath = std::string(basePathEnv);
            config->setResourceConfig(resourceConfig);
        }
        else
        {
            std::string err_msg =
                "ADSIL_RESOURCE_PATH environment variable is not set or empty. ";
            std::string info_msg = "To set the environment variable, use the following command:\n"
                                   "export ADSIL_RESOURCE_PATH=\"/absolute/path/to/resources\"";
            throw std::runtime_error(err_msg + "\n" + info_msg);
        }

        return config;
    }

    std::shared_ptr<SimulationConfig> SimulationConfig::loadFromFile(const std::string &configPath)
    {
        // For now, return default config
        // TODO: Implement JSON/XML configuration loading in future phase
        return createDefault();
    }

} // namespace simulation
