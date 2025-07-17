#include "core/ResourceLocator.hpp"
#include <filesystem>

namespace core
{
    std::string ResourceLocator::basePath_ = "."; // Default fallback

    void ResourceLocator::setBasePath(const std::string &path)
    {
        basePath_ = path;
    }

    std::string ResourceLocator::getShaderPath(const std::string &shaderBaseName, const std::string &type)
    {
        return basePath_ + "/shaders/" + shaderBaseName + "." + type + ".glsl";
    }

    std::string ResourceLocator::getJsonPath(const std::string &fileName)
    {
        return basePath_ + "/" + fileName;
    }

    std::string ResourceLocator::getJsonPathForScene(const std::string &fileName)
    {
        return basePath_ + "/extracted_frames_json/" + fileName;
    }

    std::string ResourceLocator::getModelPath(const std::string &fileName)
    {
        return basePath_ + "/models/" + fileName;
    }

    std::string ResourceLocator::getPath(const std::string &relativePath)
    {
        return basePath_ + "/" + relativePath;
    }
}
