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

        return config;
    }

    std::shared_ptr<SimulationConfig> SimulationConfig::loadFromFile(const std::string &configPath)
    {
        // For now, return default config
        // TODO: Implement JSON/XML configuration loading in future phase
        return createDefault();
    }

} // namespace simulation
