#include <adapter/AdapterManager.hpp>
#include <adapter/interfaces/IJsonAdapter.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <iostream>

using namespace adapter;

struct Dummy
{
    std::string name;
    int value;

    bool operator==(const Dummy &other) const
    {
        return name == other.name && value == other.value;
    }
};

class DummyAdapter : public IJsonAdapter<Dummy>
{
public:
    nlohmann::json toJson(const Dummy &obj) const override
    {
        return {{"name", obj.name}, {"value", obj.value}};
    }

    Dummy fromJson(const nlohmann::json &j) const override
    {
        return Dummy{j.at("name").get<std::string>(), j.at("value").get<int>()};
    }
};

void test_DummyAdapter_direct()
{
    Dummy original{"test", 99};
    DummyAdapter adapter;

    auto j = adapter.toJson(original);
    assert(j["name"] == "test");
    assert(j["value"] == 99);

    Dummy parsed = adapter.fromJson(j);
    assert(parsed == original);

    std::cout << "[PASS] DummyAdapter round-trip\n";
}

void test_DummyAdapter_with_registry()
{
    JsonAdapterRegistry registry;
    registry.registerAdapter<Dummy>(std::make_shared<DummyAdapter>());

    auto *adapter = registry.getAdapter<Dummy>();
    assert(adapter != nullptr);

    Dummy original{"unit", 123};
    auto json = adapter->toJson(original);
    Dummy reconstructed = adapter->fromJson(json);

    assert(reconstructed == original);
    std::cout << "[PASS] DummyAdapter with registry\n";
}

int main()
{
    test_DummyAdapter_direct();
    test_DummyAdapter_with_registry();
    return 0;
}
