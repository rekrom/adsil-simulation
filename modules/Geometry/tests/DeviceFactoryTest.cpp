#include <geometry/factories/DeviceFactory.hpp>
#include <geometry/implementations/Device.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

// Test assertion helpers
void assert_true(bool condition, const std::string &message)
{
    if (!condition)
    {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        exit(1);
    }
    std::cout << "✓ " << message << std::endl;
}

void assert_false(bool condition, const std::string &message)
{
    if (condition)
    {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        exit(1);
    }
    std::cout << "✓ " << message << std::endl;
}

void assert_near(float actual, float expected, float tolerance, const std::string &message)
{
    if (std::abs(actual - expected) > tolerance)
    {
        std::cerr << "ASSERTION FAILED: " << message
                  << " (expected: " << expected << ", actual: " << actual
                  << ", tolerance: " << tolerance << ")" << std::endl;
        exit(1);
    }
    std::cout << "✓ " << message << std::endl;
}

void test_deviceFactoryCreateDevices()
{
    std::cout << "\n=== Testing DeviceFactory createDevices ===" << std::endl;

    // Create test data for multiple devices
    std::vector<math::Point> origins = {
        math::Point(0, 0, 0),
        math::Point(10, 5, 2),
        math::Point(-5, 10, 1)};

    std::vector<math::Vector> directions = {
        math::Vector(1, 0, 0), // Looking along +X
        math::Vector(0, 1, 0), // Looking along +Y
        math::Vector(-1, 0, 0) // Looking along -X
    };

    std::vector<float> verticalFovs = {45.0f, 60.0f, 30.0f};
    std::vector<float> horizontalFovs = {90.0f, 120.0f, 60.0f};
    std::vector<float> ranges = {100.0f, 150.0f, 75.0f};

    // Test basic device creation
    auto devices = DeviceFactory::createDevices(
        "TestDevice",
        origins,
        directions,
        verticalFovs,
        horizontalFovs,
        ranges);

    assert_true(devices.size() == 3, "Should create 3 devices");

    // Test each device
    for (size_t i = 0; i < devices.size(); ++i)
    {
        assert_true(devices[i] != nullptr, "Device " + std::to_string(i) + " should be valid");

        assert_near(devices[i]->getVerticalFovDeg(), verticalFovs[i], 0.01f,
                    "Device " + std::to_string(i) + " should have correct vertical FOV");
        assert_near(devices[i]->getHorizontalFovDeg(), horizontalFovs[i], 0.01f,
                    "Device " + std::to_string(i) + " should have correct horizontal FOV");
        assert_near(devices[i]->getRange(), ranges[i], 0.01f,
                    "Device " + std::to_string(i) + " should have correct range");

        std::string expectedName = "TestDevice" + std::to_string(i + 1);
        assert_true(devices[i]->getName() == expectedName,
                    "Device " + std::to_string(i) + " should have correct name: " + expectedName);
    }
}

void test_deviceFactoryCreateTransmitters()
{
    std::cout << "\n=== Testing DeviceFactory createTransmitters ===" << std::endl;

    // Create transmitter data
    std::vector<math::Point> transmitterOrigins = {
        math::Point(0, 0, 5),
        math::Point(20, 0, 5)};

    std::vector<math::Vector> transmitterDirections = {
        math::Vector(0, 1, 0), // Looking forward
        math::Vector(0, -1, 0) // Looking backward
    };

    std::vector<float> transmitterVFovs = {30.0f, 45.0f};
    std::vector<float> transmitterHFovs = {60.0f, 90.0f};
    std::vector<float> transmitterRanges = {200.0f, 250.0f};

    auto transmitters = DeviceFactory::createTransmitters(
        transmitterOrigins,
        transmitterDirections,
        transmitterVFovs,
        transmitterHFovs,
        transmitterRanges);

    assert_true(transmitters.size() == 2, "Should create 2 transmitters");

    // Test transmitter naming convention
    for (size_t i = 0; i < transmitters.size(); ++i)
    {
        assert_true(transmitters[i] != nullptr, "Transmitter " + std::to_string(i) + " should be valid");

        std::string expectedName = "t" + std::to_string(i + 1);
        assert_true(transmitters[i]->getName() == expectedName,
                    "Transmitter should have correct name: " + expectedName);

        assert_near(transmitters[i]->getVerticalFovDeg(), transmitterVFovs[i], 0.01f,
                    "Transmitter " + std::to_string(i) + " should have correct vertical FOV");
        assert_near(transmitters[i]->getHorizontalFovDeg(), transmitterHFovs[i], 0.01f,
                    "Transmitter " + std::to_string(i) + " should have correct horizontal FOV");
        assert_near(transmitters[i]->getRange(), transmitterRanges[i], 0.01f,
                    "Transmitter " + std::to_string(i) + " should have correct range");
    }
}

void test_deviceFactoryCreateReceivers()
{
    std::cout << "\n=== Testing DeviceFactory createReceivers ===" << std::endl;

    // Create receiver data
    std::vector<math::Point> receiverOrigins = {
        math::Point(-10, 0, 3),
        math::Point(10, 0, 3),
        math::Point(0, 15, 3)};

    std::vector<math::Vector> receiverDirections = {
        math::Vector(1, 0, 0),  // Looking right
        math::Vector(-1, 0, 0), // Looking left
        math::Vector(0, -1, 0)  // Looking toward origin
    };

    std::vector<float> receiverVFovs = {90.0f, 90.0f, 120.0f};
    std::vector<float> receiverHFovs = {180.0f, 180.0f, 360.0f};
    std::vector<float> receiverRanges = {300.0f, 300.0f, 400.0f};

    auto receivers = DeviceFactory::createReceivers(
        receiverOrigins,
        receiverDirections,
        receiverVFovs,
        receiverHFovs,
        receiverRanges);

    assert_true(receivers.size() == 3, "Should create 3 receivers");

    // Test receiver naming convention
    for (size_t i = 0; i < receivers.size(); ++i)
    {
        assert_true(receivers[i] != nullptr, "Receiver " + std::to_string(i) + " should be valid");

        std::string expectedName = "r" + std::to_string(i + 1);
        assert_true(receivers[i]->getName() == expectedName,
                    "Receiver should have correct name: " + expectedName);

        assert_near(receivers[i]->getVerticalFovDeg(), receiverVFovs[i], 0.01f,
                    "Receiver " + std::to_string(i) + " should have correct vertical FOV");
        assert_near(receivers[i]->getHorizontalFovDeg(), receiverHFovs[i], 0.01f,
                    "Receiver " + std::to_string(i) + " should have correct horizontal FOV");
        assert_near(receivers[i]->getRange(), receiverRanges[i], 0.01f,
                    "Receiver " + std::to_string(i) + " should have correct range");
    }
}

void test_deviceFactoryWithCallback()
{
    std::cout << "\n=== Testing DeviceFactory with Callback ===" << std::endl;

    // Create test data
    std::vector<math::Point> origins = {
        math::Point(1, 2, 3),
        math::Point(4, 5, 6)};

    std::vector<math::Vector> directions = {
        math::Vector(0, 0, 1),
        math::Vector(0, 0, -1)};

    std::vector<float> vFovs = {45.0f, 60.0f};
    std::vector<float> hFovs = {90.0f, 120.0f};
    std::vector<float> ranges = {100.0f, 150.0f};

    // Track callback invocations
    std::vector<std::string> callbackResults;

    auto callback = [&callbackResults](const Device &device)
    {
        callbackResults.push_back(device.getName());
    };

    // Test createDevices with callback
    auto devices = DeviceFactory::createDevices(
        "CallbackDevice",
        origins,
        directions,
        vFovs,
        hFovs,
        ranges,
        callback);

    assert_true(devices.size() == 2, "Should create 2 devices");
    assert_true(callbackResults.size() == 2, "Callback should be called 2 times");

    assert_true(callbackResults[0] == "CallbackDevice1", "First callback should have correct device name");
    assert_true(callbackResults[1] == "CallbackDevice2", "Second callback should have correct device name");

    // Test createTransmitters with callback
    callbackResults.clear();

    auto transmitters = DeviceFactory::createTransmitters(
        origins, directions, vFovs, hFovs, ranges, callback);

    assert_true(transmitters.size() == 2, "Should create 2 transmitters");
    assert_true(callbackResults.size() == 2, "Transmitter callback should be called 2 times");

    assert_true(callbackResults[0] == "t1", "First transmitter callback should have correct name");
    assert_true(callbackResults[1] == "t2", "Second transmitter callback should have correct name");

    // Test createReceivers with callback
    callbackResults.clear();

    auto receivers = DeviceFactory::createReceivers(
        origins, directions, vFovs, hFovs, ranges, callback);

    assert_true(receivers.size() == 2, "Should create 2 receivers");
    assert_true(callbackResults.size() == 2, "Receiver callback should be called 2 times");

    assert_true(callbackResults[0] == "r1", "First receiver callback should have correct name");
    assert_true(callbackResults[1] == "r2", "Second receiver callback should have correct name");
}

void test_deviceFactoryEdgeCases()
{
    std::cout << "\n=== Testing DeviceFactory Edge Cases ===" << std::endl;

    // Test empty arrays
    std::vector<math::Point> emptyOrigins;
    std::vector<math::Vector> emptyDirections;
    std::vector<float> emptyFovs;
    std::vector<float> emptyRanges;

    auto emptyDevices = DeviceFactory::createDevices(
        "Empty", emptyOrigins, emptyDirections, emptyFovs, emptyFovs, emptyRanges);

    assert_true(emptyDevices.empty(), "Should create no devices from empty arrays");

    // Test single device
    std::vector<math::Point> singleOrigin = {math::Point(0, 0, 0)};
    std::vector<math::Vector> singleDirection = {math::Vector(1, 0, 0)};
    std::vector<float> singleFov = {45.0f};
    std::vector<float> singleRange = {100.0f};

    auto singleDevice = DeviceFactory::createDevices(
        "Single", singleOrigin, singleDirection, singleFov, singleFov, singleRange);

    assert_true(singleDevice.size() == 1, "Should create exactly one device");
    assert_true(singleDevice[0]->getName() == "Single1", "Single device should have correct name");

    // Test extreme values
    std::vector<math::Point> extremeOrigins = {math::Point(1000, -1000, 500)};
    std::vector<math::Vector> extremeDirections = {math::Vector(0, 0, 1)};
    std::vector<float> extremeVFovs = {179.0f};
    std::vector<float> extremeHFovs = {359.0f};
    std::vector<float> extremeRanges = {10000.0f};

    auto extremeDevices = DeviceFactory::createDevices(
        "Extreme", extremeOrigins, extremeDirections, extremeVFovs, extremeHFovs, extremeRanges);

    assert_true(extremeDevices.size() == 1, "Should create extreme device");
    assert_near(extremeDevices[0]->getVerticalFovDeg(), 179.0f, 0.1f, "Extreme device should preserve extreme values");
    assert_near(extremeDevices[0]->getRange(), 10000.0f, 1.0f, "Extreme device should preserve large range");
}

void test_deviceFactoryLargeScale()
{
    std::cout << "\n=== Testing DeviceFactory Large Scale Creation ===" << std::endl;

    // Create a large number of devices
    const size_t numDevices = 100;

    std::vector<math::Point> origins;
    std::vector<math::Vector> directions;
    std::vector<float> vFovs;
    std::vector<float> hFovs;
    std::vector<float> ranges;

    // Generate test data
    for (size_t i = 0; i < numDevices; ++i)
    {
        float fi = static_cast<float>(i);
        float angle = (2.0f * static_cast<float>(M_PI) * fi) / numDevices;
        origins.emplace_back(
            10.0f * std::cos(angle),
            10.0f * std::sin(angle),
            5.0f);

        directions.emplace_back(-std::cos(angle), -std::sin(angle), 0); // Point toward center
        vFovs.push_back(30.0f + static_cast<float>(i % 30));            // Varying FOV
        hFovs.push_back(60.0f + static_cast<float>(i % 60));            // Varying FOV
        ranges.push_back(50.0f + static_cast<float>(i));                // Increasing range
    }

    auto largeDeviceSet = DeviceFactory::createDevices(
        "LargeScale", origins, directions, vFovs, hFovs, ranges);

    assert_true(largeDeviceSet.size() == numDevices,
                "Should create exactly " + std::to_string(numDevices) + " devices");

    // Verify a sampling of devices
    for (size_t i = 0; i < numDevices; i += 10)
    { // Check every 10th device
        assert_true(largeDeviceSet[i] != nullptr,
                    "Large scale device " + std::to_string(i) + " should be valid");

        std::string expectedName = "LargeScale" + std::to_string(i + 1);
        assert_true(largeDeviceSet[i]->getName() == expectedName,
                    "Large scale device should have correct name");

        assert_near(largeDeviceSet[i]->getVerticalFovDeg(), vFovs[i], 0.01f,
                    "Large scale device should have correct vertical FOV");
        assert_near(largeDeviceSet[i]->getRange(), ranges[i], 0.01f,
                    "Large scale device should have correct range");
    }

    std::cout << "✓ Successfully created and verified " << numDevices << " devices" << std::endl;
}

void test_deviceFactoryMemoryManagement()
{
    std::cout << "\n=== Testing DeviceFactory Memory Management ===" << std::endl;

    std::vector<math::Point> origins = {math::Point(0, 0, 0)};
    std::vector<math::Vector> directions = {math::Vector(1, 0, 0)};
    std::vector<float> fovs = {45.0f};
    std::vector<float> ranges = {100.0f};

    // Test shared_ptr behavior
    auto devices = DeviceFactory::createDevices(
        "Memory", origins, directions, fovs, fovs, ranges);

    assert_true(devices.size() == 1, "Should create one device");

    auto device1 = devices[0];
    auto device2 = device1; // Copy shared_ptr

    assert_true(device1.get() == device2.get(), "Shared pointers should point to same object");
    assert_true(device1.use_count() >= 2, "Reference count should be at least 2");

    // Test that factory creates independent objects
    auto moreDevices = DeviceFactory::createDevices(
        "Memory", origins, directions, fovs, fovs, ranges);

    assert_true(moreDevices[0].get() != devices[0].get(),
                "Factory should create independent objects");
}

void test_deviceFactoryCallbackComplexity()
{
    std::cout << "\n=== Testing DeviceFactory Complex Callback Scenarios ===" << std::endl;

    std::vector<math::Point> origins = {
        math::Point(0, 0, 0),
        math::Point(10, 0, 0),
        math::Point(0, 10, 0)};
    std::vector<math::Vector> directions = {
        math::Vector(1, 0, 0),
        math::Vector(-1, 0, 0),
        math::Vector(0, -1, 0)};
    std::vector<float> fovs = {30.0f, 45.0f, 60.0f};
    std::vector<float> ranges = {50.0f, 75.0f, 100.0f};

    // Complex callback that modifies device after creation
    std::vector<std::string> callbackLog;

    auto complexCallback = [&callbackLog](const Device &device)
    {
        callbackLog.push_back("Created: " + device.getName());
        callbackLog.push_back("  FOV: " + std::to_string(device.getVerticalFovDeg()) + "°");
        callbackLog.push_back("  Range: " + std::to_string(device.getRange()) + " units");
    };

    auto devices = DeviceFactory::createDevices(
        "Complex", origins, directions, fovs, fovs, ranges, complexCallback);

    assert_true(devices.size() == 3, "Should create 3 devices");
    assert_true(callbackLog.size() == 9, "Callback should log 9 entries (3 per device)"); // 3 devices × 3 log entries each

    // Verify callback was called for each device
    assert_true(callbackLog[0].find("Complex1") != std::string::npos,
                "First callback should mention first device");
    assert_true(callbackLog[3].find("Complex2") != std::string::npos,
                "Second callback should mention second device");
    assert_true(callbackLog[6].find("Complex3") != std::string::npos,
                "Third callback should mention third device");
}

int main()
{
    std::cout << "🏭 Starting DeviceFactory Tests" << std::endl;
    std::cout << "================================" << std::endl;

    try
    {
        test_deviceFactoryCreateDevices();
        test_deviceFactoryCreateTransmitters();
        test_deviceFactoryCreateReceivers();
        test_deviceFactoryWithCallback();
        test_deviceFactoryEdgeCases();
        test_deviceFactoryLargeScale();
        test_deviceFactoryMemoryManagement();
        test_deviceFactoryCallbackComplexity();

        std::cout << "\n🎉 All DeviceFactory tests passed!" << std::endl;
        std::cout << "===================================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ DeviceFactory test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ DeviceFactory test failed with unknown exception" << std::endl;
        return 1;
    }
}
