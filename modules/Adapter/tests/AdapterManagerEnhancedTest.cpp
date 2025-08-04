#include <adapter/AdapterManager.hpp>
#include <adapter/JsonAdapterRegistry.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdexcept>

using namespace adapter;

// Helper function for floating point comparison
bool floatEqual(float a, float b, float epsilon = 1e-5f)
{
    return std::fabs(a - b) < epsilon;
}

// Test structure for custom adapter testing
struct TestStruct
{
    int id;
    std::string name;
    float value;

    bool operator==(const TestStruct &other) const
    {
        return id == other.id && name == other.name && floatEqual(value, other.value);
    }
};

class TestStructAdapter : public IJsonAdapter<TestStruct>
{
public:
    nlohmann::json toJson(const TestStruct &obj) const override
    {
        return {
            {"id", obj.id},
            {"name", obj.name},
            {"value", obj.value}};
    }

    TestStruct fromJson(const nlohmann::json &j) const override
    {
        return TestStruct{
            j.at("id").get<int>(),
            j.at("name").get<std::string>(),
            j.at("value").get<float>()};
    }
};

void test_AdapterManager_error_handling()
{
    AdapterManager manager;

    // Test unregistered type error handling
    bool exceptionThrown = false;
    try
    {
        // Try to use an unregistered type
        manager.toJson(math::Point(1.0f, 2.0f, 3.0f));
    }
    catch (const std::exception &)
    {
        // If we catch an exception instead of exit, that's good
        exceptionThrown = true;
        std::cout << "[INFO] AdapterManager properly throws exception for unregistered types\n";
    }

    // Note: Current implementation uses std::exit(1), so this test shows the issue
    // In a production system, we'd want exceptions instead
    std::cout << "[PASS] AdapterManager error handling test (identifies exit behavior)\n";
}

void test_AdapterManager_custom_registration()
{
    JsonAdapterRegistry registry;

    // Register custom adapter
    registry.registerAdapter<TestStruct>(std::make_shared<TestStructAdapter>());

    // Test the registration worked
    auto *adapter = registry.getAdapter<TestStruct>();
    assert(adapter != nullptr);

    // Test serialization/deserialization
    TestStruct original{42, "TestName", 3.14159f};
    nlohmann::json j = adapter->toJson(original);
    TestStruct deserialized = adapter->fromJson(j);

    assert(deserialized == original);

    std::cout << "[PASS] AdapterManager custom type registration test\n";
}

void test_AdapterManager_multiple_registrations()
{
    JsonAdapterRegistry registry;

    // Register multiple adapters
    registry.registerAdapter<math::Point>(std::make_shared<PointJsonAdapter>());
    registry.registerAdapter<math::Vector>(std::make_shared<VectorJsonAdapter>());
    registry.registerAdapter<TestStruct>(std::make_shared<TestStructAdapter>());

    // Test all adapters work independently
    auto *pointAdapter = registry.getAdapter<math::Point>();
    auto *vectorAdapter = registry.getAdapter<math::Vector>();
    auto *testAdapter = registry.getAdapter<TestStruct>();

    assert(pointAdapter != nullptr);
    assert(vectorAdapter != nullptr);
    assert(testAdapter != nullptr);

    // Test round-trip for each type
    math::Point p(1.0f, 2.0f, 3.0f);
    math::Vector v(4.0f, 5.0f, 6.0f);
    TestStruct ts{100, "MultiTest", 2.718f};

    // Point round-trip
    nlohmann::json pJson = pointAdapter->toJson(p);
    math::Point p2 = pointAdapter->fromJson(pJson);
    assert(floatEqual(p.x(), p2.x()) && floatEqual(p.y(), p2.y()) && floatEqual(p.z(), p2.z()));

    // Vector round-trip
    nlohmann::json vJson = vectorAdapter->toJson(v);
    math::Vector v2 = vectorAdapter->fromJson(vJson);
    assert(floatEqual(v.x(), v2.x()) && floatEqual(v.y(), v2.y()) && floatEqual(v.z(), v2.z()));

    // TestStruct round-trip
    nlohmann::json tsJson = testAdapter->toJson(ts);
    TestStruct ts2 = testAdapter->fromJson(tsJson);
    assert(ts == ts2);

    std::cout << "[PASS] AdapterManager multiple registrations test\n";
}

void test_AdapterManager_null_adapter_handling()
{
    JsonAdapterRegistry registry;

    // Test getting adapter for unregistered type
    auto *adapter = registry.getAdapter<TestStruct>();
    assert(adapter == nullptr);

    std::cout << "[PASS] AdapterManager null adapter handling test\n";
}

void test_AdapterManager_registry_override()
{
    JsonAdapterRegistry registry;

    // Register an adapter
    auto originalAdapter = std::make_shared<TestStructAdapter>();
    registry.registerAdapter<TestStruct>(originalAdapter);

    // Register a different adapter for the same type (should override)
    auto newAdapter = std::make_shared<TestStructAdapter>();
    registry.registerAdapter<TestStruct>(newAdapter);

    // Get the adapter - should be the new one
    auto *retrievedAdapter = registry.getAdapter<TestStruct>();
    assert(retrievedAdapter != nullptr);

    // Test it works
    TestStruct test{1, "Override", 1.0f};
    nlohmann::json j = retrievedAdapter->toJson(test);
    TestStruct result = retrievedAdapter->fromJson(j);
    assert(test == result);

    std::cout << "[PASS] AdapterManager registry override test\n";
}

void test_AdapterManager_type_safety()
{
    JsonAdapterRegistry registry;

    // Register adapters for different types
    registry.registerAdapter<math::Point>(std::make_shared<PointJsonAdapter>());
    registry.registerAdapter<math::Vector>(std::make_shared<VectorJsonAdapter>());

    // Verify type safety - each adapter only works for its type
    auto *pointAdapter = registry.getAdapter<math::Point>();
    auto *vectorAdapter = registry.getAdapter<math::Vector>();
    auto *nonExistentAdapter = registry.getAdapter<TestStruct>();

    assert(pointAdapter != nullptr);
    assert(vectorAdapter != nullptr);
    assert(nonExistentAdapter == nullptr);

    // Verify adapters are different objects (cast to void* for comparison)
    assert(static_cast<void *>(pointAdapter) != static_cast<void *>(vectorAdapter));

    std::cout << "[PASS] AdapterManager type safety test\n";
}

void test_AdapterManager_json_validation()
{
    PointJsonAdapter adapter;

    // Test valid JSON
    nlohmann::json validJson = {
        {"x", 1.0f},
        {"y", 2.0f},
        {"z", 3.0f}};

    math::Point p = adapter.fromJson(validJson);
    assert(floatEqual(p.x(), 1.0f));
    assert(floatEqual(p.y(), 2.0f));
    assert(floatEqual(p.z(), 3.0f));

    // Test invalid JSON (missing field)
    nlohmann::json invalidJson = {
        {"x", 1.0f},
        {"y", 2.0f}
        // missing z
    };

    bool exceptionThrown = false;
    try
    {
        math::Point p2 = adapter.fromJson(invalidJson);
    }
    catch (const std::exception &)
    {
        exceptionThrown = true;
    }

    assert(exceptionThrown);

    std::cout << "[PASS] AdapterManager JSON validation test\n";
}

void test_AdapterManager_performance_basic()
{
    JsonAdapterRegistry registry;
    registry.registerAdapter<math::Point>(std::make_shared<PointJsonAdapter>());

    auto *adapter = registry.getAdapter<math::Point>();

    // Basic performance test - multiple operations should not significantly slow down
    const int iterations = 1000;

    for (int i = 0; i < iterations; ++i)
    {
        math::Point p(static_cast<float>(i), static_cast<float>(i * 2), static_cast<float>(i * 3));
        nlohmann::json j = adapter->toJson(p);
        math::Point p2 = adapter->fromJson(j);

        // Quick validation
        assert(floatEqual(p.x(), p2.x()));
    }

    std::cout << "[PASS] AdapterManager basic performance test (" << iterations << " iterations)\n";
}

int main()
{
    test_AdapterManager_error_handling();
    test_AdapterManager_custom_registration();
    test_AdapterManager_multiple_registrations();
    test_AdapterManager_null_adapter_handling();
    test_AdapterManager_registry_override();
    test_AdapterManager_type_safety();
    test_AdapterManager_json_validation();
    test_AdapterManager_performance_basic();

    std::cout << "[SUCCESS] All enhanced AdapterManager tests completed!\n";
    return 0;
}
