#pragma once

#include <string>

namespace core
{
    class ResourceLocator
    {
    public:
        // Set the base resource directory (called once during app initialization)
        static void setBasePath(const std::string &path);

        // Get the full path to a shader file (e.g. "ground", "vert")
        static std::string getShaderPath(const std::string &shaderBaseName, const std::string &type);

        // Get the full path to a JSON file (e.g. "car.json")
        static std::string getJsonPath(const std::string &fileName);

        // Get the full path to a model file (e.g. "cube.obj")
        static std::string getModelPath(const std::string &fileName);

        // General-purpose path resolver (e.g. "shaders/ground.vert.glsl")
        static std::string getPath(const std::string &relativePath);

    private:
        static std::string basePath_;
    };
}
