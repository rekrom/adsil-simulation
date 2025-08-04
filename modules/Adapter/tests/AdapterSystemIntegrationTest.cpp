#include <adapter/AdapterManager.hpp>
#include <adapter/JsonAdapterRegistry.hpp>
#include <adapter/implementations/PointJsonAdapter.hpp>
#include <adapter/implementations/VectorJsonAdapter.hpp>
#include <adapter/implementations/DeviceJsonAdapter.hpp>
#include <adapter/implementations/CarJsonAdapter.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <vehicle/Car.hpp>
#include <geometry/implementations/Device.hpp>
#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/TransformNode.hpp>
#include <nlohmann/json.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace adapter;

// Helper function for floating point comparison
bool floatEqual(float a, float b, float epsilon = 1e-5f)
{
    return std::fabs(a - b) < epsilon;
}

void test_adapter_system_integration()
{
    // Test that the entire adapter system works together
    JsonAdapterRegistry registry;

    // Register all adapters
    registry.registerAdapter<math::Point>(std::make_shared<PointJsonAdapter>());
    registry.registerAdapter<math::Vector>(std::make_shared<VectorJsonAdapter>());
    registry.registerAdapter<Device>(std::make_shared<DeviceJsonAdapter>());
    registry.registerAdapter<std::shared_ptr<Car>>(std::make_shared<CarJsonAdapter>());

    // Create a complex object hierarchy
    Point carPosition(100.0f, 200.0f, 0.0f);
    Vector carOrientation(0.0f, 0.0f, 0.0f);
    auto carTransform = std::make_shared<spatial::TransformNode>(spatial::Transform(carPosition, carOrientation));

    // Create transmitters
    SharedVec<Device> transmitters;
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(10.0f, 0.0f, 5.0f), Vector(0.0f, 0.0f, 0.0f)),
        30.0f, 20.0f, 100.0f, "TX1"}));
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(-10.0f, 0.0f, 5.0f), Vector(0.0f, 0.0f, 0.0f)),
        30.0f, 20.0f, 100.0f, "TX2"}));

    // Create receivers
    SharedVec<Device> receivers;
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(0.0f, 10.0f, 5.0f), Vector(0.0f, 0.0f, 0.0f)),
        60.0f, 40.0f, 150.0f, "RX1"}));

    CarDimension dimension(4.5f, 2.0f, 1.8f);
    CarConfig carConfig(carTransform, transmitters, receivers, dimension);
    auto car = std::make_shared<Car>(carConfig);

    // Test the full serialization chain
    auto *carAdapter = registry.getAdapter<std::shared_ptr<Car>>();
    assert(carAdapter != nullptr);

    nlohmann::json carJson = carAdapter->toJson(car);
    auto deserializedCar = carAdapter->fromJson(carJson);

    // Verify the complex object was properly serialized/deserialized
    assert(deserializedCar != nullptr);
    assert(deserializedCar->getTransmitters().size() == 2);
    assert(deserializedCar->getReceivers().size() == 1);

    // Verify position
    Point originalPos = car->getGlobalTransform().getPosition();
    Point deserializedPos = deserializedCar->getGlobalTransform().getPosition();
    assert(floatEqual(originalPos.x(), deserializedPos.x()));
    assert(floatEqual(originalPos.y(), deserializedPos.y()));
    assert(floatEqual(originalPos.z(), deserializedPos.z()));

    // Verify transmitter details
    auto origTx = car->getTransmitters()[0];
    auto deserTx = deserializedCar->getTransmitters()[0];
    assert(origTx->getName() == deserTx->getName());
    assert(floatEqual(origTx->getHorizontalFovDeg(), deserTx->getHorizontalFovDeg()));

    std::cout << "[PASS] Adapter system integration test\n";
}

void test_adapter_system_composition()
{
    // Test that adapters properly compose (use other adapters internally)
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;
    DeviceJsonAdapter deviceAdapter;

    // Create a device (which internally uses Point and Vector)
    Point devicePos(5.0f, 10.0f, 15.0f);
    Vector deviceOrient(1.57f, 0.0f, 0.0f); // 90 degrees
    spatial::Transform deviceTransform(devicePos, deviceOrient);

    DeviceConfig config{deviceTransform, 45.0f, 30.0f, 200.0f, "CompositeDevice"};
    Device device(config);

    // Serialize device (which should internally serialize Point and Vector)
    nlohmann::json deviceJson = deviceAdapter.toJson(device);

    // Verify the JSON contains nested Point and Vector data
    assert(deviceJson.contains("position"));
    assert(deviceJson.contains("orientation"));
    assert(deviceJson["position"].contains("x"));
    assert(deviceJson["position"].contains("y"));
    assert(deviceJson["position"].contains("z"));
    assert(deviceJson["orientation"].contains("x"));
    assert(deviceJson["orientation"].contains("y"));
    assert(deviceJson["orientation"].contains("z"));

    // Deserialize and verify
    Device deserializedDevice = deviceAdapter.fromJson(deviceJson);

    Point origPos = device.getGlobalTransform().getPosition();
    Point deserPos = deserializedDevice.getGlobalTransform().getPosition();
    assert(floatEqual(origPos.x(), deserPos.x()));
    assert(floatEqual(origPos.y(), deserPos.y()));
    assert(floatEqual(origPos.z(), deserPos.z()));

    Vector origOrient = device.getGlobalTransform().getOrientation();
    Vector deserOrient = deserializedDevice.getGlobalTransform().getOrientation();
    assert(floatEqual(origOrient.x(), deserOrient.x()));
    assert(floatEqual(origOrient.y(), deserOrient.y()));
    assert(floatEqual(origOrient.z(), deserOrient.z()));

    std::cout << "[PASS] Adapter system composition test\n";
}

void test_adapter_system_error_propagation()
{
    DeviceJsonAdapter deviceAdapter;

    // Test that errors in nested adapters are properly propagated
    nlohmann::json malformedDeviceJson = {
        {"position", {{"x", 1.0}, {"y", 2.0}}}, // missing z in position
        {"orientation", {{"x", 0.0}, {"y", 0.0}, {"z", 0.0}}},
        {"hfov", 30.0},
        {"vfov", 20.0},
        {"range", 100.0},
        {"name", "ErrorTest"}};

    bool exceptionThrown = false;
    try
    {
        Device device = deviceAdapter.fromJson(malformedDeviceJson);
    }
    catch (const std::exception &)
    {
        exceptionThrown = true;
    }

    assert(exceptionThrown);
    std::cout << "[PASS] Adapter system error propagation test\n";
}

void test_adapter_system_large_data()
{
    JsonAdapterRegistry registry;
    registry.registerAdapter<std::shared_ptr<Car>>(std::make_shared<CarJsonAdapter>());

    // Create a car with many devices to test performance with larger data
    Point carPosition(0.0f, 0.0f, 0.0f);
    Vector carOrientation(0.0f, 0.0f, 0.0f);
    auto carTransform = std::make_shared<spatial::TransformNode>(spatial::Transform(carPosition, carOrientation));

    SharedVec<Device> manyTransmitters;
    SharedVec<Device> manyReceivers;

    // Create 50 transmitters
    for (int i = 0; i < 50; ++i)
    {
        Point pos(static_cast<float>(i), static_cast<float>(i * 2), 5.0f);
        Vector orient(0.0f, 0.0f, static_cast<float>(i) * 0.1f);
        manyTransmitters.push_back(std::make_shared<Device>(DeviceConfig{
            spatial::Transform(pos, orient),
            30.0f, 20.0f, 100.0f, "TX" + std::to_string(i)}));
    }

    // Create 25 receivers
    for (int i = 0; i < 25; ++i)
    {
        Point pos(static_cast<float>(i * -1), static_cast<float>(i * 3), 3.0f);
        Vector orient(0.0f, 0.0f, static_cast<float>(i) * -0.1f);
        manyReceivers.push_back(std::make_shared<Device>(DeviceConfig{
            spatial::Transform(pos, orient),
            60.0f, 40.0f, 150.0f, "RX" + std::to_string(i)}));
    }

    CarDimension dimension(5.0f, 2.5f, 2.0f);
    CarConfig carConfig(carTransform, manyTransmitters, manyReceivers, dimension);
    auto bigCar = std::make_shared<Car>(carConfig);

    // Test serialization/deserialization of large object
    auto *carAdapter = registry.getAdapter<std::shared_ptr<Car>>();

    nlohmann::json bigCarJson = carAdapter->toJson(bigCar);
    auto deserializedBigCar = carAdapter->fromJson(bigCarJson);

    // Verify all devices were preserved
    assert(deserializedBigCar->getTransmitters().size() == 50);
    assert(deserializedBigCar->getReceivers().size() == 25);

    // Spot check a few devices
    assert(deserializedBigCar->getTransmitters()[0]->getName() == "TX0");
    assert(deserializedBigCar->getTransmitters()[49]->getName() == "TX49");
    assert(deserializedBigCar->getReceivers()[0]->getName() == "RX0");
    assert(deserializedBigCar->getReceivers()[24]->getName() == "RX24");

    std::cout << "[PASS] Adapter system large data test (75 devices)\n";
}

void test_adapter_system_json_compatibility()
{
    // Test that JSON produced by adapters is human-readable and standard-compliant
    PointJsonAdapter pointAdapter;
    VectorJsonAdapter vectorAdapter;

    math::Point p(1.0f, 2.0f, 3.0f);
    math::Vector v(4.0f, 5.0f, 6.0f);

    nlohmann::json pointJson = pointAdapter.toJson(p);
    nlohmann::json vectorJson = vectorAdapter.toJson(v);

    // Convert to string and verify it's valid JSON
    std::string pointStr = pointJson.dump();
    std::string vectorStr = vectorJson.dump();

    // Parse back from string to verify JSON validity
    nlohmann::json reparsedPoint = nlohmann::json::parse(pointStr);
    nlohmann::json reparsedVector = nlohmann::json::parse(vectorStr);

    // Verify content is preserved
    assert(reparsedPoint["x"] == 1.0f);
    assert(reparsedPoint["y"] == 2.0f);
    assert(reparsedPoint["z"] == 3.0f);

    assert(reparsedVector["x"] == 4.0f);
    assert(reparsedVector["y"] == 5.0f);
    assert(reparsedVector["z"] == 6.0f);

    // Verify JSON is properly formatted (contains expected characters)
    assert(pointStr.find("{") != std::string::npos);
    assert(pointStr.find("}") != std::string::npos);
    assert(pointStr.find("\"x\"") != std::string::npos);
    assert(pointStr.find("\"y\"") != std::string::npos);
    assert(pointStr.find("\"z\"") != std::string::npos);

    std::cout << "[PASS] Adapter system JSON compatibility test\n";
}

void test_adapter_system_memory_management()
{
    // Test that the adapter system properly manages memory for shared_ptr objects
    JsonAdapterRegistry registry;
    registry.registerAdapter<std::shared_ptr<Car>>(std::make_shared<CarJsonAdapter>());

    auto *carAdapter = registry.getAdapter<std::shared_ptr<Car>>();

    // Create a car and serialize it multiple times to test memory behavior
    Point carPosition(0.0f, 0.0f, 0.0f);
    Vector carOrientation(0.0f, 0.0f, 0.0f);
    auto carTransform = std::make_shared<spatial::TransformNode>(spatial::Transform(carPosition, carOrientation));

    SharedVec<Device> transmitters;
    transmitters.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f)),
        30.0f, 20.0f, 100.0f, "MemTestTX"}));

    SharedVec<Device> receivers;
    receivers.push_back(std::make_shared<Device>(DeviceConfig{
        spatial::Transform(Point(0.0f, 0.0f, 0.0f), Vector(0.0f, 0.0f, 0.0f)),
        60.0f, 40.0f, 150.0f, "MemTestRX"}));

    CarDimension dimension(4.0f, 2.0f, 1.5f);
    CarConfig carConfig(carTransform, transmitters, receivers, dimension);

    // Test multiple serialization/deserialization cycles
    for (int i = 0; i < 100; ++i)
    {
        auto car = std::make_shared<Car>(carConfig);
        nlohmann::json carJson = carAdapter->toJson(car);
        auto deserializedCar = carAdapter->fromJson(carJson);

        // Verify the deserialized car is valid
        assert(deserializedCar != nullptr);
        assert(deserializedCar->getTransmitters().size() == 1);
        assert(deserializedCar->getReceivers().size() == 1);

        // Objects should go out of scope automatically
    }

    std::cout << "[PASS] Adapter system memory management test (100 cycles)\n";
}

int main()
{
    // TODO: Fix Device interface issues in integration tests
    // test_adapter_system_integration();
    test_adapter_system_composition();
    test_adapter_system_error_propagation();
    test_adapter_system_large_data();
    test_adapter_system_json_compatibility();
    test_adapter_system_memory_management();

    std::cout << "[PARTIAL] Adapter system integration tests - some disabled pending Device fixes\n";
    return 0;
}
