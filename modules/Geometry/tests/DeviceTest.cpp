#include <geometry/implementations/Device.hpp>
#include <geometry/configs/DeviceConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <math/PointCloud.hpp>
#include <iostream>
#include <cassert>
#include <cmath>

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

void test_deviceConstruction()
{
    std::cout << "\n=== Testing Device Construction ===" << std::endl;

    // Test device construction with basic config
    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {
        transform,
        60.0f,  // vertical FOV in degrees
        90.0f,  // horizontal FOV in degrees
        100.0f, // range
        "TestDevice"};

    Device device(config);

    assert_near(device.getVerticalFovDeg(), 60.0f, 0.01f, "Vertical FOV should match config");
    assert_near(device.getHorizontalFovDeg(), 90.0f, 0.01f, "Horizontal FOV should match config");
    assert_near(device.getRange(), 100.0f, 0.01f, "Range should match config");
    assert_true(device.getName() == "TestDevice", "Name should match config");

    std::string deviceStr = device.toString();
    assert_true(deviceStr.find("Device") != std::string::npos,
                "toString should contain 'Device'");
    assert_true(deviceStr.find("TestDevice") != std::string::npos ||
                    deviceStr.find("60") != std::string::npos,
                "toString should contain device information");
}

void test_deviceFovConversions()
{
    std::cout << "\n=== Testing Device FOV Conversions ===" << std::endl;

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {transform, 45.0f, 60.0f, 50.0f, "FOVTestDevice"};

    Device device(config);

    // Test degree/radian conversions for vertical FOV
    float verticalFovRad = device.getVerticalFovRad();
    float expectedVerticalRad = 45.0f * static_cast<float>(M_PI) / 180.0f;
    assert_near(verticalFovRad, expectedVerticalRad, 0.01f,
                "Vertical FOV in radians should match converted value");

    // Test degree/radian conversions for horizontal FOV
    float horizontalFovRad = device.getHorizontalFovRad();
    float expectedHorizontalRad = 60.0f * static_cast<float>(M_PI) / 180.0f;
    assert_near(horizontalFovRad, expectedHorizontalRad, 0.01f,
                "Horizontal FOV in radians should match converted value");

    // Test setters with degree values
    device.setVerticalFovDeg(30.0f);
    device.setHorizontalFovDeg(120.0f);

    assert_near(device.getVerticalFovDeg(), 30.0f, 0.01f,
                "Setting vertical FOV in degrees should work");
    assert_near(device.getHorizontalFovDeg(), 120.0f, 0.01f,
                "Setting horizontal FOV in degrees should work");

    // Test setters with radian values
    float newVerticalRad = 45.0f * static_cast<float>(M_PI) / 180.0f;
    float newHorizontalRad = 90.0f * static_cast<float>(M_PI) / 180.0f;

    device.setVerticalFovRad(newVerticalRad);
    device.setHorizontalFovRad(newHorizontalRad);

    assert_near(device.getVerticalFovRad(), newVerticalRad, 0.01f,
                "Setting vertical FOV in radians should work");
    assert_near(device.getHorizontalFovRad(), newHorizontalRad, 0.01f,
                "Setting horizontal FOV in radians should work");

    assert_near(device.getVerticalFovDeg(), 45.0f, 0.1f,
                "Radian setter should affect degree getter");
    assert_near(device.getHorizontalFovDeg(), 90.0f, 0.1f,
                "Radian setter should affect degree getter");
}

void test_deviceRangeOperations()
{
    std::cout << "\n=== Testing Device Range Operations ===" << std::endl;

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {transform, 45.0f, 45.0f, 75.0f, "RangeTestDevice"};

    Device device(config);

    assert_near(device.getRange(), 75.0f, 0.01f, "Initial range should match config");

    // Test range setter
    device.setRange(150.0f);
    assert_near(device.getRange(), 150.0f, 0.01f, "Range setter should update value");

    // Test edge cases
    device.setRange(0.1f);
    assert_near(device.getRange(), 0.1f, 0.01f, "Very small range should be preserved");

    device.setRange(1000.0f);
    assert_near(device.getRange(), 1000.0f, 0.01f, "Large range should be preserved");
}

void test_devicePointsInFov()
{
    std::cout << "\n=== Testing Device Points in FOV ===" << std::endl;

    // Create a device looking forward (along +Z axis)
    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {
        transform,
        60.0f, // 60-degree vertical FOV
        60.0f, // 60-degree horizontal FOV
        10.0f, // 10 unit range
        "FOVDevice"};

    Device device(config);

    // Create a point cloud with various points
    math::PointCloud testCloud;

    // Points that should be in FOV
    testCloud.addPoint(math::Point(0, 0, 5));   // Straight ahead, center
    testCloud.addPoint(math::Point(1, 1, 8));   // Slightly off-center, within FOV
    testCloud.addPoint(math::Point(-1, -1, 7)); // Slightly off-center, within FOV

    // Points that should be outside FOV
    testCloud.addPoint(math::Point(10, 0, 5)); // Too far to the side
    testCloud.addPoint(math::Point(0, 10, 5)); // Too far up/down
    testCloud.addPoint(math::Point(0, 0, -5)); // Behind the device
    testCloud.addPoint(math::Point(0, 0, 15)); // Beyond range

    auto visiblePoints = device.pointsInFov(testCloud);

    assert_true(visiblePoints != nullptr, "pointsInFov should return a valid point cloud");

    // Should have some points but not all
    assert_true(visiblePoints->getPoints().size() > 0, "Should detect some points in FOV");
    assert_true(visiblePoints->getPoints().size() < testCloud.getPoints().size(),
                "Should filter out some points");

    std::cout << "✓ FOV filtering: " << visiblePoints->getPoints().size()
              << "/" << testCloud.getPoints().size() << " points visible" << std::endl;
}

void test_deviceWithTransforms()
{
    std::cout << "\n=== Testing Device with Transforms ===" << std::endl;

    // Test device with translation
    spatial::Transform translatedTransform({5, 10, 2}, {0, 0, 0});
    DeviceConfig config = {translatedTransform, 45.0f, 45.0f, 20.0f, "TranslatedDevice"};

    Device translatedDevice(config);

    // Create point cloud relative to translated position
    math::PointCloud testCloud;
    testCloud.addPoint(math::Point(5, 10, 12)); // Ahead of translated device
    testCloud.addPoint(math::Point(15, 10, 2)); // To the side of translated device
    testCloud.addPoint(math::Point(5, 10, -8)); // Behind translated device

    auto visiblePoints = translatedDevice.pointsInFov(testCloud);
    assert_true(visiblePoints != nullptr, "Translated device should process point cloud");

    // Test device with rotation
    spatial::Transform rotatedTransform({0, 0, 0}, {0, 45, 0}); // 45-degree Y rotation
    DeviceConfig rotatedConfig = {rotatedTransform, 60.0f, 60.0f, 15.0f, "RotatedDevice"};

    Device rotatedDevice(rotatedConfig);

    math::PointCloud rotatedTestCloud;
    rotatedTestCloud.addPoint(math::Point(5, 0, 5));  // Should be in rotated FOV
    rotatedTestCloud.addPoint(math::Point(0, 0, 10)); // Should be outside rotated FOV

    auto rotatedVisiblePoints = rotatedDevice.pointsInFov(rotatedTestCloud);
    assert_true(rotatedVisiblePoints != nullptr, "Rotated device should process point cloud");
}

void test_deviceEdgeCases()
{
    std::cout << "\n=== Testing Device Edge Cases ===" << std::endl;

    // Test device with very narrow FOV
    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig narrowConfig = {transform, 1.0f, 1.0f, 50.0f, "NarrowDevice"};

    Device narrowDevice(narrowConfig);

    assert_near(narrowDevice.getVerticalFovDeg(), 1.0f, 0.01f,
                "Narrow vertical FOV should be preserved");
    assert_near(narrowDevice.getHorizontalFovDeg(), 1.0f, 0.01f,
                "Narrow horizontal FOV should be preserved");

    // Test with very wide FOV
    DeviceConfig wideConfig = {transform, 179.0f, 179.0f, 100.0f, "WideDevice"};

    Device wideDevice(wideConfig);

    assert_near(wideDevice.getVerticalFovDeg(), 179.0f, 0.01f,
                "Wide vertical FOV should be preserved");
    assert_near(wideDevice.getHorizontalFovDeg(), 179.0f, 0.01f,
                "Wide horizontal FOV should be preserved");

    // Test with very short range
    DeviceConfig shortRangeConfig = {transform, 45.0f, 45.0f, 0.1f, "ShortRangeDevice"};

    Device shortRangeDevice(shortRangeConfig);
    assert_near(shortRangeDevice.getRange(), 0.1f, 0.01f,
                "Very short range should be preserved");

    // Test with very long range
    DeviceConfig longRangeConfig = {transform, 45.0f, 45.0f, 10000.0f, "LongRangeDevice"};

    Device longRangeDevice(longRangeConfig);
    assert_near(longRangeDevice.getRange(), 10000.0f, 0.1f,
                "Very long range should be preserved");
}

void test_deviceEmptyPointCloud()
{
    std::cout << "\n=== Testing Device with Empty Point Cloud ===" << std::endl;

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {transform, 60.0f, 60.0f, 100.0f, "EmptyTestDevice"};

    Device device(config);

    // Test with empty point cloud
    math::PointCloud emptyCloud;
    auto result = device.pointsInFov(emptyCloud);

    assert_true(result != nullptr, "pointsInFov should handle empty point cloud");
    assert_true(result->getPoints().empty(), "Result should be empty for empty input");
}

void test_deviceParameterModification()
{
    std::cout << "\n=== Testing Device Parameter Modification ===" << std::endl;

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    DeviceConfig config = {transform, 30.0f, 40.0f, 25.0f, "ModifiableDevice"};

    Device device(config);

    // Test multiple parameter changes
    device.setVerticalFovDeg(70.0f);
    device.setHorizontalFovDeg(110.0f);
    device.setRange(200.0f);

    assert_near(device.getVerticalFovDeg(), 70.0f, 0.01f,
                "Modified vertical FOV should be updated");
    assert_near(device.getHorizontalFovDeg(), 110.0f, 0.01f,
                "Modified horizontal FOV should be updated");
    assert_near(device.getRange(), 200.0f, 0.01f,
                "Modified range should be updated");

    // Test radian/degree consistency after modifications
    float verticalRad = device.getVerticalFovRad();
    float expectedRad = 70.0f * static_cast<float>(M_PI) / 180.0f;
    assert_near(verticalRad, expectedRad, 0.01f,
                "Radian values should be consistent after degree modification");

    // Test modifying via radians
    device.setVerticalFovRad(static_cast<float>(M_PI) / 4.0f); // 45 degrees
    assert_near(device.getVerticalFovDeg(), 45.0f, 0.1f,
                "Degree value should update when set via radians");
}

void test_deviceToString()
{
    std::cout << "\n=== Testing Device toString Method ===" << std::endl;

    spatial::Transform transform({1.5f, 2.5f, 3.5f}, {10, 20, 30});
    DeviceConfig config = {transform, 45.0f, 90.0f, 150.0f, "StringTestDevice"};

    Device device(config);
    std::string deviceStr = device.toString();

    assert_true(deviceStr.find("Device") != std::string::npos,
                "toString should contain 'Device'");

    // Should contain position information
    assert_true(deviceStr.find("1.5") != std::string::npos || deviceStr.find("origin") != std::string::npos,
                "toString should contain position information");

    // Should contain FOV information
    assert_true(deviceStr.find("45") != std::string::npos || deviceStr.find("90") != std::string::npos ||
                    deviceStr.find("FOV") != std::string::npos,
                "toString should contain FOV information");

    std::cout << "✓ Device toString output: " << deviceStr << std::endl;
}

void test_deviceComplexFovScenario()
{
    std::cout << "\n=== Testing Device Complex FOV Scenario ===" << std::endl;

    // Create a realistic LIDAR-like device
    spatial::Transform lidarTransform({0, 0, 2}, {0, 0, 0}); // 2 units high
    DeviceConfig lidarConfig = {
        lidarTransform,
        30.0f,  // 30-degree vertical FOV
        360.0f, // Note: This might be clamped, but let's test
        50.0f,  // 50 unit range
        "LIDARDevice"};

    Device lidarDevice(lidarConfig);

    // Test the wide horizontal FOV (may be clamped)
    float actualHorizontalFov = lidarDevice.getHorizontalFovDeg();
    assert_true(actualHorizontalFov > 0, "Horizontal FOV should be positive");

    // Create a complex point cloud scenario
    math::PointCloud complexCloud;

    // Add points at various distances and angles
    for (int i = 0; i < 20; ++i)
    {
        float angle = static_cast<float>(i) * 18.0f;    // 18-degree increments
        float distance = 10.0f + static_cast<float>(i); // Varying distances

        float x = distance * std::cos(angle * static_cast<float>(M_PI) / 180.0f);
        float y = distance * std::sin(angle * static_cast<float>(M_PI) / 180.0f);
        float z = 2.0f; // Same height as device

        complexCloud.addPoint(math::Point(x, y, z));
    }

    auto visibleComplex = lidarDevice.pointsInFov(complexCloud);
    assert_true(visibleComplex != nullptr, "Complex FOV test should return valid result");

    std::cout << "✓ Complex FOV test: " << visibleComplex->getPoints().size()
              << "/" << complexCloud.getPoints().size() << " points visible" << std::endl;
}

int main()
{
    std::cout << "📡 Starting Device Tests" << std::endl;
    std::cout << "========================" << std::endl;

    try
    {
        test_deviceConstruction();
        test_deviceFovConversions();
        test_deviceRangeOperations();
        test_devicePointsInFov();
        test_deviceWithTransforms();
        test_deviceEdgeCases();
        test_deviceEmptyPointCloud();
        test_deviceParameterModification();
        test_deviceToString();
        test_deviceComplexFovScenario();

        std::cout << "\n🎉 All Device tests passed!" << std::endl;
        std::cout << "===========================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Device test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Device test failed with unknown exception" << std::endl;
        return 1;
    }
}
