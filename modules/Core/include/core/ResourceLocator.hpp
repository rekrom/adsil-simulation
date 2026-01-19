#pragma once

#include <string>

namespace core
{
    class ResourceLocator
    {
    public:
        // Set the base resource directory (called once during app initialization)
        static void setBasePath(const std::string &path)
        {
            basePath_ = path;
        }

        // Get the full path to a shader file (e.g. "ground", "vert")
        static std::string getShaderPath(const std::string &shaderBaseName, const std::string &type)
        {
            return basePath_ + "/shaders/" + shaderBaseName + "." + type + ".glsl";
        }

        // Get the full path to a JSON file (e.g. "car.json")
        static std::string getJsonPath(const std::string &fileName)
        {
            return basePath_ + "/" + fileName;
        }

        // Get the full path to a JSON file for point cloud scene (e.g. "extracted/frames_json/frame_00000.json")
        static std::string getJsonPathForScene(const std::string &fileName)
        {
            return basePath_ + "/extracted_frames_json/" + fileName;
        }

        // Get the full path to a model file (e.g. "cube.obj")
        static std::string getModelPath(const std::string &fileName)
        {
            return basePath_ + "/models/" + fileName;
        }

        static std::string getLoggingPath(const std::string &fileName)
        {
            return basePath_ + "/logs/" + fileName;
        }

        // Get the export directory path for CSV data files
        static std::string getExportPath()
        {
            return basePath_ + "/exports";
        }

        // General-purpose path resolver (e.g. "shaders/ground.vert.glsl")
        static std::string getPath(const std::string &relativePath)
        {
            return basePath_ + "/" + relativePath;
        }

    private:
        static inline std::string basePath_ = "."; // Default fallback - using inline for header-only
    };
}
