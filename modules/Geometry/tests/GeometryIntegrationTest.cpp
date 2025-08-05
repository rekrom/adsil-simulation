#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <geometry/implementations/Device.hpp>
#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/factories/DeviceFactory.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <geometry/configs/DeviceConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <math/PointCloud.hpp>
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

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

void test_geometryShapeInteroperability()
{
    std::cout << "\n=== Testing Shape Interoperability ===" << std::endl;

    // Create shapes using factory
    CubeDimension cubeDim(4.0f);
    spatial::Transform cubeTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig cubeConfig = {cubeTransform, cubeDim, "InteropCube"};

    CylinderDimension cylinderDim(6.0f, 2.0f);
    spatial::Transform cylinderTransform({10, 0, 0}, {0, 0, 0});
    CylinderConfig cylinderConfig = {cylinderTransform, cylinderDim, "InteropCylinder"};

    auto cube = ShapeFactory::createCube(cubeConfig);
    auto cylinder = ShapeFactory::createCylinder(cylinderConfig);

    // Store as base class pointers
    std::vector<std::shared_ptr<ShapeBase>> shapes = {cube, cylinder};

    // Test polymorphic behavior
    for (auto &shape : shapes)
    {
        auto wireframe = shape->wireframe();
        auto mesh = shape->surfaceMesh(128);
        auto description = shape->toString();

        assert_true(wireframe.size() > 0, "Shape should generate wireframe");
        assert_true(mesh != nullptr, "Shape should generate mesh");
        assert_true(mesh->getPoints().size() > 0, "Shape mesh should have points");
        assert_true(!description.empty(), "Shape should have description");
    }

    // Test that different shapes generate different wireframe sizes
    auto cubeWireframe = cube->wireframe();
    auto cylinderWireframe = cylinder->wireframe();

    assert_true(cubeWireframe.size() == 24, "Cube should have 24 wireframe points");
    assert_true(cylinderWireframe.size() == 32, "Cylinder should have 32 wireframe points");
}

void test_deviceShapeInteraction()
{
    std::cout << "\n=== Testing Device-Shape Interaction ===" << std::endl;

    // Create a scene with shapes and devices

    // Create some shapes to provide point clouds
    CubeDimension cubeDim(2.0f);
    spatial::Transform cubeTransform({5, 0, 0}, {0, 0, 0});
    CubeConfig cubeConfig = {cubeTransform, cubeDim, "SceneCube"};
    auto sceneCube = ShapeFactory::createCube(cubeConfig);

    CylinderDimension cylinderDim(3.0f, 1.5f);
    spatial::Transform cylinderTransform({-5, 0, 0}, {0, 0, 0});
    CylinderConfig cylinderConfig = {cylinderTransform, cylinderDim, "SceneCylinder"};
    auto sceneCylinder = ShapeFactory::createCylinder(cylinderConfig);

    // Create a combined point cloud from shapes
    auto cubeMesh = sceneCube->surfaceMesh(64);
    auto cylinderMesh = sceneCylinder->surfaceMesh(64);

    math::PointCloud sceneCloud;
    for (const auto &point : cubeMesh->getPoints())
    {
        sceneCloud.addPoint(point);
    }
    for (const auto &point : cylinderMesh->getPoints())
    {
        sceneCloud.addPoint(point);
    }

    assert_true(sceneCloud.getPoints().size() > 0, "Scene should have combined points");

    // Create a device to observe the scene
    spatial::Transform deviceTransform({0, -10, 0}, {0, 90, 0}); // Looking toward scene from -Y
    DeviceConfig deviceConfig = {deviceTransform, 60.0f, 120.0f, 20.0f, "SceneObserver"};
    Device sceneDevice(deviceConfig);

    // Test device observing the scene
    auto visiblePoints = sceneDevice.pointsInFov(sceneCloud);

    assert_true(visiblePoints != nullptr, "Device should process scene point cloud");
    assert_true(visiblePoints->getPoints().size() <= sceneCloud.getPoints().size(),
                "Visible points should not exceed total points");

    std::cout << "✓ Scene observation: " << visiblePoints->getPoints().size()
              << "/" << sceneCloud.getPoints().size() << " points visible" << std::endl;
}

void test_multiDeviceScenario()
{
    std::cout << "\n=== Testing Multi-Device Scenario ===" << std::endl;

    // Create a surveillance-like scenario with multiple devices

    // Create target objects
    CubeDimension targetDim(1.0f);
    std::vector<std::shared_ptr<ShapeBase>> targets;

    for (int i = 0; i < 5; ++i)
    {
        float fi = static_cast<float>(i);
        spatial::Transform targetTransform({fi * 3.0f, fi * 2.0f, 0}, {0, 0, fi * 15.0f});
        CubeConfig targetConfig = {targetTransform, targetDim, "Target" + std::to_string(fi)};
        targets.push_back(ShapeFactory::createCube(targetConfig));
    }

    // Create a combined target point cloud
    math::PointCloud targetsCloud;
    for (auto &target : targets)
    {
        auto targetMesh = target->surfaceMesh(32);
        for (const auto &point : targetMesh->getPoints())
        {
            targetsCloud.addPoint(point);
        }
    }

    // Create multiple observer devices using factory
    std::vector<math::Point> deviceOrigins = {
        math::Point(-10, -10, 5), // Southwest observer
        math::Point(10, -10, 5),  // Southeast observer
        math::Point(0, 20, 5)     // North observer
    };

    std::vector<math::Vector> deviceDirections = {
        math::Vector(1, 1, -0.2f),  // Looking northeast and slightly down
        math::Vector(-1, 1, -0.2f), // Looking northwest and slightly down
        math::Vector(0, -1, -0.2f)  // Looking south and slightly down
    };

    std::vector<float> deviceVFovs = {45.0f, 45.0f, 60.0f};
    std::vector<float> deviceHFovs = {90.0f, 90.0f, 120.0f};
    std::vector<float> deviceRanges = {30.0f, 30.0f, 40.0f};

    auto observers = DeviceFactory::createDevices(
        "Observer", deviceOrigins, deviceDirections, deviceVFovs, deviceHFovs, deviceRanges);

    assert_true(observers.size() == 3, "Should create 3 observer devices");

    // Test each observer's view of the targets
    std::vector<size_t> visibleCounts;
    for (auto &observer : observers)
    {
        auto visibleTargets = observer->pointsInFov(targetsCloud);
        visibleCounts.push_back(visibleTargets->getPoints().size());

        assert_true(visibleTargets != nullptr, "Observer should process target cloud");
    }

    // At least one observer should see something
    size_t totalVisible = 0;
    for (size_t count : visibleCounts)
    {
        totalVisible += count;
    }

    std::cout << "✓ Multi-device observation: Total visible points = " << totalVisible << std::endl;
    std::cout << "✓ Observer visibility: [" << visibleCounts[0] << ", "
              << visibleCounts[1] << ", " << visibleCounts[2] << "]" << std::endl;
}

void test_geometryComplexTransforms()
{
    std::cout << "\n=== Testing Complex Transform Scenarios ===" << std::endl;

    // Test shapes and devices with complex transform hierarchies

    // Create parent transform
    spatial::Transform parentTransform({10, 20, 5}, {45, 0, 90});

    // Create child shapes with relative transforms
    CubeDimension cubeDim(2.0f);
    spatial::Transform relativeCubeTransform({2, 0, 1}, {0, 45, 0});
    CubeConfig cubeConfig = {relativeCubeTransform, cubeDim, "ComplexCube"};
    auto complexCube = ShapeFactory::createCube(cubeConfig);

    // Test that transform affects shape generation
    auto wireframe = complexCube->wireframe();
    auto mesh = complexCube->surfaceMesh(64);

    assert_true(wireframe.size() == 24, "Complex transform cube should generate wireframe");
    assert_true(mesh->getPoints().size() > 0, "Complex transform cube should generate mesh");

    // Create device with complex transform
    DeviceConfig deviceConfig = {parentTransform, 60.0f, 90.0f, 25.0f, "ComplexDevice"};
    Device complexDevice(deviceConfig);

    // Test device functionality with complex transform
    math::PointCloud testCloud;
    testCloud.addPoint(math::Point(10, 20, 10)); // Point in front of rotated device
    testCloud.addPoint(math::Point(5, 15, 5));   // Point to the side
    testCloud.addPoint(math::Point(15, 25, 0));  // Point at different height

    auto visiblePoints = complexDevice.pointsInFov(testCloud);
    assert_true(visiblePoints != nullptr, "Complex transform device should process points");

    std::cout << "✓ Complex transforms: Device sees " << visiblePoints->getPoints().size()
              << "/" << testCloud.getPoints().size() << " points" << std::endl;
}

void test_geometryPerformanceIntegration()
{
    std::cout << "\n=== Testing Geometry Performance Integration ===" << std::endl;

    // Test performance with multiple shapes and devices
    const size_t numShapes = 20;
    const size_t numDevices = 10;

    // Create multiple shapes
    std::vector<std::shared_ptr<ShapeBase>> shapes;

    for (size_t i = 0; i < numShapes; ++i)
    {
        if (i % 2 == 0)
        {
            // Create cubes
            float fi = static_cast<float>(i);
            CubeDimension dim(1.0f + fi * 0.1f);
            spatial::Transform transform({fi * 2.0f, static_cast<float>(i % 4) * 3.0f, 0}, {0, 0, fi * 18.0f});
            CubeConfig config = {transform, dim, "PerfCube" + std::to_string(fi)};
            shapes.push_back(ShapeFactory::createCube(config));
        }
        else
        {
            // Create cylinders
            float fi = static_cast<float>(i);
            CylinderDimension dim(2.0f + fi * 0.1f, 0.5f + fi * 0.05f);
            spatial::Transform transform({fi * 2.0f, static_cast<float>(i % 4) * 3.0f, 0}, {0, 0, fi * 18.0f});
            CylinderConfig config = {transform, dim, "PerfCylinder" + std::to_string(fi)};
            shapes.push_back(ShapeFactory::createCylinder(config));
        }
    }

    assert_true(shapes.size() == numShapes, "Should create all performance test shapes");

    // Generate combined point cloud
    math::PointCloud performanceCloud;
    for (auto &shape : shapes)
    {
        auto mesh = shape->surfaceMesh(32); // Lower quality for performance
        for (const auto &point : mesh->getPoints())
        {
            performanceCloud.addPoint(point);
        }
    }

    std::cout << "✓ Performance scene: " << performanceCloud.getPoints().size()
              << " total points from " << numShapes << " shapes" << std::endl;

    // Create multiple devices
    std::vector<math::Point> origins;
    std::vector<math::Vector> directions;
    std::vector<float> vFovs, hFovs, ranges;

    for (size_t i = 0; i < numDevices; ++i)
    {
        float angle = (2.0f * static_cast<float>(M_PI) * static_cast<float>(i)) / static_cast<float>(numDevices);
        origins.emplace_back(15.0f * std::cos(angle), 15.0f * std::sin(angle), 5.0f);
        directions.emplace_back(-std::cos(angle), -std::sin(angle), -0.1f);
        vFovs.push_back(45.0f);
        hFovs.push_back(60.0f);
        ranges.push_back(25.0f);
    }

    auto devices = DeviceFactory::createDevices(
        "PerfDevice", origins, directions, vFovs, hFovs, ranges);

    assert_true(devices.size() == numDevices, "Should create all performance test devices");

    // Test all devices against the scene
    size_t totalVisiblePoints = 0;
    for (auto &device : devices)
    {
        auto visiblePoints = device->pointsInFov(performanceCloud);
        totalVisiblePoints += visiblePoints->getPoints().size();
    }

    std::cout << "✓ Performance test: " << totalVisiblePoints
              << " total visible points across " << numDevices << " devices" << std::endl;
}

void test_geometryErrorHandling()
{
    std::cout << "\n=== Testing Geometry Error Handling ===" << std::endl;

    // Test various edge cases and potential error conditions

    // Test with zero-dimension shapes
    CubeDimension zeroCubeDim(0.0f);
    spatial::Transform zeroTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig zeroConfig = {zeroTransform, zeroCubeDim, "ZeroErrorCube"};

    auto zeroCube = ShapeFactory::createCube(zeroConfig);
    assert_true(zeroCube != nullptr, "Should handle zero-dimension cube");

    auto zeroWireframe = zeroCube->wireframe();
    auto zeroMesh = zeroCube->surfaceMesh(16);

    assert_true(zeroWireframe.size() == 24, "Zero cube should still generate wireframe structure");
    assert_true(zeroMesh != nullptr, "Zero cube should generate mesh object");

    // Test device with extreme FOV
    DeviceConfig extremeConfig = {zeroTransform, 179.0f, 359.0f, 1000.0f, "ExtremeDevice"};
    Device extremeDevice(extremeConfig);

    math::PointCloud testCloud;
    testCloud.addPoint(math::Point(1, 0, 0));
    testCloud.addPoint(math::Point(0, 1, 0));
    testCloud.addPoint(math::Point(0, 0, 1));

    auto extremeResult = extremeDevice.pointsInFov(testCloud);
    assert_true(extremeResult != nullptr, "Extreme device should handle point cloud");

    // Test empty point cloud
    math::PointCloud emptyCloud;
    auto emptyResult = extremeDevice.pointsInFov(emptyCloud);
    assert_true(emptyResult != nullptr, "Device should handle empty point cloud");
    assert_true(emptyResult->getPoints().empty(), "Empty input should produce empty result");
}

void test_geometryFactoryConsistency()
{
    std::cout << "\n=== Testing Factory Consistency ===" << std::endl;

    // Test that factories produce consistent results

    // Create identical configurations
    CubeDimension cubeDim(3.0f);
    spatial::Transform cubeTransform({1, 2, 3}, {10, 20, 30});
    CubeConfig cubeConfig = {cubeTransform, cubeDim, "ConsistentCube"};

    auto cube1 = ShapeFactory::createCube(cubeConfig);
    auto cube2 = ShapeFactory::createCube(cubeConfig);

    // Test that they are independent objects
    assert_true(cube1.get() != cube2.get(), "Factory should create independent objects");

    // Test that they have identical properties
    auto cube1Ptr = std::dynamic_pointer_cast<Cube>(cube1);
    auto cube2Ptr = std::dynamic_pointer_cast<Cube>(cube2);

    auto dim1 = cube1Ptr->getDimension();
    auto dim2 = cube2Ptr->getDimension();

    assert_near(dim1.length, dim2.length, 0.001f, "Identical configs should produce identical dimensions");
    assert_near(dim1.width, dim2.width, 0.001f, "Identical configs should produce identical dimensions");
    assert_near(dim1.height, dim2.height, 0.001f, "Identical configs should produce identical dimensions");

    // Test device factory consistency
    std::vector<math::Point> origins = {math::Point(0, 0, 0)};
    std::vector<math::Vector> directions = {math::Vector(1, 0, 0)};
    std::vector<float> fovs = {45.0f};
    std::vector<float> ranges = {100.0f};

    auto devices1 = DeviceFactory::createDevices("Test", origins, directions, fovs, fovs, ranges);
    auto devices2 = DeviceFactory::createDevices("Test", origins, directions, fovs, fovs, ranges);

    assert_true(devices1.size() == devices2.size(), "Device factory should be consistent");
    assert_true(devices1[0].get() != devices2[0].get(), "Should create independent device objects");

    assert_near(devices1[0]->getRange(), devices2[0]->getRange(), 0.001f,
                "Identical device configs should produce identical properties");
}

int main()
{
    std::cout << "🔮 Starting Geometry Integration Tests" << std::endl;
    std::cout << "=======================================" << std::endl;

    try
    {
        test_geometryShapeInteroperability();
        test_deviceShapeInteraction();
        test_multiDeviceScenario();
        test_geometryComplexTransforms();
        test_geometryPerformanceIntegration();
        test_geometryErrorHandling();
        test_geometryFactoryConsistency();

        std::cout << "\n🎉 All Geometry Integration tests passed!" << std::endl;
        std::cout << "=========================================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Geometry Integration test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Geometry Integration test failed with unknown exception" << std::endl;
        return 1;
    }
}
