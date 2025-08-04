#pragma once

#include <adapter/JsonAdapterRegistry.hpp>
#include <utils/JsonFileUtils.hpp>

namespace adapter
{
    class AdapterManager
    {
    public:
        AdapterManager();

        template <typename T>
        T fromJson(const std::string &path)
        {
            nlohmann::json j = utils::loadJsonOrExit(path);
            auto *adapter = registry_.getAdapter<T>();
            if (!adapter)
            {
                std::cerr << "[ERROR] Adapter not registered for: " << typeid(T).name() << std::endl;
                throw std::runtime_error("Adapter not registered for type: " + std::string(typeid(T).name()));
            }
            return adapter->fromJson(j);
        }

        template <typename T>
        nlohmann::json toJson(const T &object)
        {
            auto *adapter = registry_.getAdapter<T>();
            if (!adapter)
            {
                std::cerr << "[ERROR] Adapter not registered for: " << typeid(T).name() << std::endl;
                throw std::runtime_error("Adapter not registered for type: " + std::string(typeid(T).name()));
            }
            return adapter->toJson(object);
        }

    private:
        JsonAdapterRegistry registry_;
    };

}
