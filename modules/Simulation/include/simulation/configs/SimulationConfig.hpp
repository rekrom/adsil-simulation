#pragma once

#include <string>
#include <memory>
#include <glm/vec3.hpp>

namespace simulation
{
    /**
     * @brief Configuration class for SimulationManager
     *
     * This class centralizes all configuration parameters for the simulation,
     * replacing hardcoded values throughout the codebase.
     */
    class SimulationConfig
    {
    public:
        // Window configuration
        struct WindowConfig
        {
            int width = 1280;
            int height = 720;
            std::string title = "ADSIL Analyzer - OpenGL";
        };

        // Frame buffer configuration
        struct FrameConfig
        {
            int bufferWindowSize = 3; // ±3 frame window (total = 7)
        };

        // Point cloud configuration
        struct PointCloudConfig
        {
            float pointSize = 2.0f;
            float alpha = 1.0f;
            glm::vec3 detectedColor = glm::vec3(110.0f / 255.0f, 1.0f, 160.0f / 255.0f);
            std::string outsideName = "PointCloudEntity - Outside Point Cloud";
            std::string detectedName = "PointCloudEntity - Selected Point Cloud";
        };

        // Car entity configuration
        struct CarConfig
        {
            glm::vec3 color = glm::vec3(0.2f, 0.6f, 0.9f);
        };

        // Resource configuration
        struct ResourceConfig
        {
            std::string basePath = "/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources";
            std::string sceneFile = "scene.json";
        };

        // Static factory methods
        static std::shared_ptr<SimulationConfig> createDefault();
        static std::shared_ptr<SimulationConfig> loadFromFile(const std::string &configPath);

        // Getters
        const WindowConfig &getWindowConfig() const { return windowConfig_; }
        const FrameConfig &getFrameConfig() const { return frameConfig_; }
        const PointCloudConfig &getPointCloudConfig() const { return pointCloudConfig_; }
        const CarConfig &getCarConfig() const { return carConfig_; }
        const ResourceConfig &getResourceConfig() const { return resourceConfig_; }

        // Setters for runtime configuration
        void setWindowConfig(const WindowConfig &config) { windowConfig_ = config; }
        void setFrameConfig(const FrameConfig &config) { frameConfig_ = config; }
        void setPointCloudConfig(const PointCloudConfig &config) { pointCloudConfig_ = config; }
        void setCarConfig(const CarConfig &config) { carConfig_ = config; }
        void setResourceConfig(const ResourceConfig &config) { resourceConfig_ = config; }

    private:
        WindowConfig windowConfig_;
        FrameConfig frameConfig_;
        PointCloudConfig pointCloudConfig_;
        CarConfig carConfig_;
        ResourceConfig resourceConfig_;
    };

} // namespace simulation
