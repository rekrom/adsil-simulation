#pragma once

#include <adapter/IJsonAdapterBase.hpp> // yeni base class
#include <adapter/IJsonAdapter.hpp>     // yeni base class
#include <memory>
#include <typeindex>
#include <unordered_map>
namespace adapter
{
    class JsonAdapterRegistry
    {
    public:
        template <typename T>
        void registerAdapter(std::shared_ptr<IJsonAdapter<T>> adapter)
        {
            registry_[std::type_index(typeid(T))] = std::move(adapter);
        }

        template <typename T>
        IJsonAdapter<T> *getAdapter() const
        {
            auto it = registry_.find(std::type_index(typeid(T)));
            if (it == registry_.end())
                return nullptr;

            // Artık dynamic_cast geçerli çünkü base class polymorphic
            return dynamic_cast<IJsonAdapter<T> *>(it->second.get());
        }

    private:
        std::unordered_map<std::type_index, std::shared_ptr<IJsonAdapterBase>> registry_;
    };

}
