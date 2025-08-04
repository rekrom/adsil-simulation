#include <geometry/implementations/Cube.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <math/Point.hpp>
#include <math/Vector.hpp>
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

void test_cubeConstruction()
{
    std::cout << "\n=== Testing Cube Construction ===" << std::endl;

    // Test default cube construction
    CubeDimension defaultDim(2.0f);
    spatial::Transform defaultTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig config = {defaultTransform, defaultDim, "TestCube"};

    Cube cube(config);

    assert_true(cube.toString().find("TestCube") != std::string::npos ||
                    cube.toString().find("Cube") != std::string::npos,
                "Cube toString should contain identifying information");

    auto dimensions = cube.getDimension();
    assert_near(dimensions.length, 2.0f, 0.01f, "Cube length should match config");
    assert_near(dimensions.width, 2.0f, 0.01f, "Cube width should match config");
    assert_near(dimensions.height, 2.0f, 0.01f, "Cube height should match config");
}

void test_cubeDimensionStruct()
{
    std::cout << "\n=== Testing CubeDimension Struct ===" << std::endl;

    // Test default constructor
    CubeDimension defaultDim;
    assert_near(defaultDim.length, 0.0f, 0.01f, "Default dimension length should be 0");
    assert_near(defaultDim.width, 0.0f, 0.01f, "Default dimension width should be 0");
    assert_near(defaultDim.height, 0.0f, 0.01f, "Default dimension height should be 0");

    // Test parameterized constructor
    CubeDimension sizeDim(5.0f);
    assert_near(sizeDim.length, 5.0f, 0.01f, "Parameterized dimension length should match");
    assert_near(sizeDim.width, 5.0f, 0.01f, "Parameterized dimension width should match");
    assert_near(sizeDim.height, 5.0f, 0.01f, "Parameterized dimension height should match");

    // Test toString method
    std::string dimStr = sizeDim.toString();
    assert_true(dimStr.find("CubeDimension") != std::string::npos,
                "CubeDimension toString should contain class name");
    assert_true(dimStr.find("5") != std::string::npos,
                "CubeDimension toString should contain dimension values");
}

void test_cubeWireframe()
{
    std::cout << "\n=== Testing Cube Wireframe Generation ===" << std::endl;

    // Create a unit cube at origin
    CubeDimension unitDim(2.0f); // 2x2x2 cube
    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig config = {originTransform, unitDim, "UnitCube"};

    Cube cube(config);
    auto wireframe = cube.wireframe();

    // A cube wireframe should have 24 points (12 edges × 2 points per edge)
    assert_true(wireframe.size() == 24, "Cube wireframe should have 24 points (12 edges)");

    // Test that wireframe points are within expected bounds
    float maxExpected = 1.0f + 0.1f; // Half the cube size + tolerance
    float minExpected = -1.0f - 0.1f;

    for (const auto &point : wireframe)
    {
        assert_true(point.x() >= minExpected && point.x() <= maxExpected,
                    "Wireframe X coordinates should be within cube bounds");
        assert_true(point.y() >= minExpected && point.y() <= maxExpected,
                    "Wireframe Y coordinates should be within cube bounds");
        assert_true(point.z() >= minExpected && point.z() <= maxExpected,
                    "Wireframe Z coordinates should be within cube bounds");
    }
}

void test_cubeSurfaceMesh()
{
    std::cout << "\n=== Testing Cube Surface Mesh Generation ===" << std::endl;

    // Create a unit cube
    CubeDimension unitDim(2.0f);
    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig config = {originTransform, unitDim, "MeshCube"};

    Cube cube(config);

    // Test different quality levels
    auto lowQualityMesh = cube.surfaceMesh(64);    // Low quality
    auto highQualityMesh = cube.surfaceMesh(2048); // High quality

    assert_true(lowQualityMesh != nullptr, "Low quality mesh should be created");
    assert_true(highQualityMesh != nullptr, "High quality mesh should be created");

    assert_true(lowQualityMesh->getPoints().size() > 0, "Low quality mesh should have points");
    assert_true(highQualityMesh->getPoints().size() > 0, "High quality mesh should have points");

    // Higher quality should have more points
    assert_true(highQualityMesh->getPoints().size() >= lowQualityMesh->getPoints().size(),
                "High quality mesh should have at least as many points as low quality");

    // Test mesh points are within reasonable bounds
    float maxBound = 1.1f; // Slightly larger than cube half-size
    for (const auto &point : highQualityMesh->getPoints())
    {
        assert_true(std::abs(point.x()) <= maxBound, "Mesh X coordinates should be within bounds");
        assert_true(std::abs(point.y()) <= maxBound, "Mesh Y coordinates should be within bounds");
        assert_true(std::abs(point.z()) <= maxBound, "Mesh Z coordinates should be within bounds");
    }
}

void test_cubeTransforms()
{
    std::cout << "\n=== Testing Cube with Transforms ===" << std::endl;

    // Test cube with translation
    CubeDimension unitDim(2.0f);
    spatial::Transform translatedTransform({5.0f, 3.0f, -2.0f}, {0, 0, 0});
    CubeConfig config = {translatedTransform, unitDim, "TranslatedCube"};

    Cube translatedCube(config);
    auto wireframe = translatedCube.wireframe();

    // Check that wireframe points are translated correctly
    bool foundPointNearOrigin = false;
    for (const auto &point : wireframe)
    {
        if (std::abs(point.x() - 5.0f) < 2.0f &&
            std::abs(point.y() - 3.0f) < 2.0f &&
            std::abs(point.z() + 2.0f) < 2.0f)
        {
            foundPointNearOrigin = true;
            break;
        }
    }
    assert_true(foundPointNearOrigin, "Translated cube should have points near the translated origin");

    // Test cube with rotation
    spatial::Transform rotatedTransform({0, 0, 0}, {0, 0, 45.0f}); // 45-degree Z rotation
    CubeConfig rotatedConfig = {rotatedTransform, unitDim, "RotatedCube"};

    Cube rotatedCube(rotatedConfig);
    auto rotatedWireframe = rotatedCube.wireframe();

    assert_true(rotatedWireframe.size() == 24, "Rotated cube should still have 24 wireframe points");
}

void test_cubeEdgeCases()
{
    std::cout << "\n=== Testing Cube Edge Cases ===" << std::endl;

    // Test very small cube
    CubeDimension tinyDim(0.01f);
    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig tinyConfig = {originTransform, tinyDim, "TinyCube"};

    Cube tinyCube(tinyConfig);
    auto tinyWireframe = tinyCube.wireframe();
    auto tinyMesh = tinyCube.surfaceMesh(16);

    assert_true(tinyWireframe.size() == 24, "Tiny cube should still have 24 wireframe points");
    assert_true(tinyMesh->getPoints().size() > 0, "Tiny cube should still generate mesh points");

    // Test large cube
    CubeDimension largeDim(100.0f);
    CubeConfig largeConfig = {originTransform, largeDim, "LargeCube"};

    Cube largeCube(largeConfig);
    auto largeWireframe = largeCube.wireframe();
    auto largeMesh = largeCube.surfaceMesh(64);

    assert_true(largeWireframe.size() == 24, "Large cube should still have 24 wireframe points");
    assert_true(largeMesh->getPoints().size() > 0, "Large cube should still generate mesh points");

    // Check that large cube points are within expected large bounds
    float largeBound = 50.1f; // Half the large cube size + tolerance
    for (const auto &point : largeWireframe)
    {
        assert_true(std::abs(point.x()) <= largeBound, "Large cube X coordinates should be within large bounds");
        assert_true(std::abs(point.y()) <= largeBound, "Large cube Y coordinates should be within large bounds");
        assert_true(std::abs(point.z()) <= largeBound, "Large cube Z coordinates should be within large bounds");
    }
}

void test_cubeMeshQuality()
{
    std::cout << "\n=== Testing Cube Mesh Quality Scaling ===" << std::endl;

    CubeDimension unitDim(2.0f);
    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig config = {originTransform, unitDim, "QualityCube"};

    Cube cube(config);

    // Test various quality levels
    auto mesh4 = cube.surfaceMesh(4);
    auto mesh16 = cube.surfaceMesh(16);
    auto mesh64 = cube.surfaceMesh(64);
    auto mesh256 = cube.surfaceMesh(256);

    assert_true(mesh4->getPoints().size() > 0, "Quality 4 mesh should have points");
    assert_true(mesh16->getPoints().size() > 0, "Quality 16 mesh should have points");
    assert_true(mesh64->getPoints().size() > 0, "Quality 64 mesh should have points");
    assert_true(mesh256->getPoints().size() > 0, "Quality 256 mesh should have points");

    // Higher quality should generally have more points (though not strictly monotonic due to face-based generation)
    assert_true(mesh256->getPoints().size() >= mesh4->getPoints().size(),
                "High quality mesh should have at least as many points as very low quality");

    std::cout << "✓ Mesh quality scaling: "
              << "Q4=" << mesh4->getPoints().size() << " points, "
              << "Q16=" << mesh16->getPoints().size() << " points, "
              << "Q64=" << mesh64->getPoints().size() << " points, "
              << "Q256=" << mesh256->getPoints().size() << " points" << std::endl;
}

void test_cubeToString()
{
    std::cout << "\n=== Testing Cube toString Method ===" << std::endl;

    CubeDimension customDim(3.5f);
    spatial::Transform transform({1, 2, 3}, {10, 20, 30});
    CubeConfig config = {transform, customDim, "CustomCube"};

    Cube cube(config);
    std::string cubeStr = cube.toString();

    assert_true(cubeStr.find("Cube") != std::string::npos,
                "toString should contain 'Cube'");
    assert_true(cubeStr.find("3.5") != std::string::npos || cubeStr.find("dim") != std::string::npos,
                "toString should contain dimension information");

    std::cout << "✓ Cube toString output: " << cubeStr << std::endl;
}

void test_cubeNonUniformDimensions()
{
    std::cout << "\n=== Testing Cube with Non-Uniform Handling ===" << std::endl;

    // Note: The current implementation uses height for all dimensions in mesh generation
    // This tests the behavior with different dimension values
    CubeDimension nonUniform;
    nonUniform.length = 2.0f;
    nonUniform.width = 3.0f;
    nonUniform.height = 4.0f; // This is what the implementation actually uses

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    CubeConfig config = {transform, nonUniform, "NonUniformCube"};

    Cube cube(config);

    auto dimensions = cube.getDimension();
    assert_near(dimensions.length, 2.0f, 0.01f, "Non-uniform cube should preserve length");
    assert_near(dimensions.width, 3.0f, 0.01f, "Non-uniform cube should preserve width");
    assert_near(dimensions.height, 4.0f, 0.01f, "Non-uniform cube should preserve height");

    // Test that mesh generation still works
    auto mesh = cube.surfaceMesh(64);
    assert_true(mesh->getPoints().size() > 0, "Non-uniform cube should generate mesh");

    // Test wireframe generation
    auto wireframe = cube.wireframe();
    assert_true(wireframe.size() == 24, "Non-uniform cube should generate 24 wireframe points");
}

int main()
{
    std::cout << "🔷 Starting Cube Tests" << std::endl;
    std::cout << "======================" << std::endl;

    try
    {
        test_cubeConstruction();
        test_cubeDimensionStruct();
        test_cubeWireframe();
        test_cubeSurfaceMesh();
        test_cubeTransforms();
        test_cubeEdgeCases();
        test_cubeMeshQuality();
        test_cubeToString();
        test_cubeNonUniformDimensions();

        std::cout << "\n🎉 All Cube tests passed!" << std::endl;
        std::cout << "=========================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Cube test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Cube test failed with unknown exception" << std::endl;
        return 1;
    }
}
