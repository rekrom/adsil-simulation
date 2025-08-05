#include <geometry/implementations/Cylinder.hpp>
#include <geometry/configs/CylinderConfig.hpp>
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

// Read the CylinderConfig structure first
void test_cylinderConfigStructure()
{
    std::cout << "\n=== Testing CylinderConfig Structure ===" << std::endl;

    // Create a basic cylinder config
    CylinderDimension dim;
    dim.radius_ = 2.5f;
    dim.height_ = 5.0f;

    spatial::Transform transform({1, 2, 3}, {0, 0, 45});
    CylinderConfig config = {transform, dim, "TestCylinder"};

    assert_near(config.dimension.radius_, 2.5f, 0.01f, "Config should preserve radius");
    assert_near(config.dimension.height_, 5.0f, 0.01f, "Config should preserve height");
    assert_true(config.name == "TestCylinder", "Config should preserve name");
}

void test_cylinderConstruction()
{
    std::cout << "\n=== Testing Cylinder Construction ===" << std::endl;

    // Test cylinder construction
    CylinderDimension dim;
    dim.radius_ = 3.0f;
    dim.height_ = 6.0f;

    spatial::Transform defaultTransform({0, 0, 0}, {0, 0, 0});
    CylinderConfig config = {defaultTransform, dim, "TestCylinder"};

    Cylinder cylinder(config);

    assert_near(cylinder.getRadius(), 3.0f, 0.01f, "Cylinder radius should match config");
    assert_near(cylinder.getHeight(), 6.0f, 0.01f, "Cylinder height should match config");

    std::string cylinderStr = cylinder.toString();
    assert_true(cylinderStr.find("Cylinder") != std::string::npos,
                "toString should contain 'Cylinder'");
    assert_true(cylinderStr.find("3") != std::string::npos,
                "toString should contain radius value");
    assert_true(cylinderStr.find("6") != std::string::npos,
                "toString should contain height value");
}

void test_cylinderWireframe()
{
    std::cout << "\n=== Testing Cylinder Wireframe Generation ===" << std::endl;

    // Create a unit cylinder
    CylinderDimension dim;
    dim.radius_ = 1.0f;
    dim.height_ = 2.0f;

    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CylinderConfig config = {originTransform, dim, "WireframeCylinder"};

    Cylinder cylinder(config);
    auto wireframe = cylinder.wireframe();

    // Wireframe should have 32 points (16 segments × 2 points per segment for top/bottom circles)
    assert_true(wireframe.size() == 32, "Cylinder wireframe should have 32 points (16 segments)");

    // Test that wireframe points are within expected cylindrical bounds
    float radiusTolerance = 1.1f; // Radius + tolerance
    float heightTolerance = 1.1f; // Half height + tolerance

    for (const auto &point : wireframe)
    {
        // Check radial distance from Z-axis
        float radialDistance = std::sqrt(point.x() * point.x() + point.y() * point.y());
        assert_true(radialDistance <= radiusTolerance,
                    "Wireframe points should be within cylindrical radius bounds");

        // Check height bounds
        assert_true(std::abs(point.z()) <= heightTolerance,
                    "Wireframe Z coordinates should be within height bounds");
    }
}

void test_cylinderSurfaceMesh()
{
    std::cout << "\n=== Testing Cylinder Surface Mesh Generation ===" << std::endl;

    // Create a cylinder for mesh testing
    CylinderDimension dim;
    dim.radius_ = 2.0f;
    dim.height_ = 4.0f;

    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CylinderConfig config = {originTransform, dim, "MeshCylinder"};

    Cylinder cylinder(config);

    // Test different quality levels
    auto lowQualityMesh = cylinder.surfaceMesh(8);     // Very low quality
    auto mediumQualityMesh = cylinder.surfaceMesh(64); // Medium quality
    auto highQualityMesh = cylinder.surfaceMesh(512);  // High quality

    assert_true(lowQualityMesh != nullptr, "Low quality mesh should be created");
    assert_true(mediumQualityMesh != nullptr, "Medium quality mesh should be created");
    assert_true(highQualityMesh != nullptr, "High quality mesh should be created");

    assert_true(lowQualityMesh->getPoints().size() > 0, "Low quality mesh should have points");
    assert_true(mediumQualityMesh->getPoints().size() > 0, "Medium quality mesh should have points");
    assert_true(highQualityMesh->getPoints().size() > 0, "High quality mesh should have points");

    // Higher quality should have more points
    assert_true(highQualityMesh->getPoints().size() >= lowQualityMesh->getPoints().size(),
                "High quality mesh should have at least as many points as low quality");

    // Test mesh points are within cylindrical bounds
    float radiusBound = 2.1f; // Radius + tolerance
    float heightBound = 2.1f; // Half height + tolerance

    for (const auto &point : mediumQualityMesh->getPoints())
    {
        float radialDistance = std::sqrt(point.x() * point.x() + point.y() * point.y());
        assert_true(radialDistance <= radiusBound, "Mesh points should be within radius bounds");
        assert_true(std::abs(point.z()) <= heightBound, "Mesh points should be within height bounds");
    }

    std::cout << "✓ Mesh point counts: Low=" << lowQualityMesh->getPoints().size()
              << ", Medium=" << mediumQualityMesh->getPoints().size()
              << ", High=" << highQualityMesh->getPoints().size() << std::endl;
}

void test_cylinderTransforms()
{
    std::cout << "\n=== Testing Cylinder with Transforms ===" << std::endl;

    // Test cylinder with translation
    CylinderDimension dim;
    dim.radius_ = 1.5f;
    dim.height_ = 3.0f;

    spatial::Transform translatedTransform({10.0f, -5.0f, 2.0f}, {0, 0, 0});
    CylinderConfig config = {translatedTransform, dim, "TranslatedCylinder"};

    Cylinder translatedCylinder(config);
    auto wireframe = translatedCylinder.wireframe();

    // Check that wireframe points are translated correctly
    bool foundPointNearTranslatedCenter = false;
    for (const auto &point : wireframe)
    {
        float distanceFromCenter = std::sqrt(
            (point.x() - 10.0f) * (point.x() - 10.0f) +
            (point.y() + 5.0f) * (point.y() + 5.0f));
        if (distanceFromCenter <= 2.0f && std::abs(point.z() - 2.0f) <= 2.0f)
        {
            foundPointNearTranslatedCenter = true;
            break;
        }
    }
    assert_true(foundPointNearTranslatedCenter,
                "Translated cylinder should have points near the translated center");

    // Test cylinder with rotation
    spatial::Transform rotatedTransform({0, 0, 0}, {90, 0, 0}); // 90-degree X rotation
    CylinderConfig rotatedConfig = {rotatedTransform, dim, "RotatedCylinder"};

    Cylinder rotatedCylinder(rotatedConfig);
    auto rotatedWireframe = rotatedCylinder.wireframe();

    assert_true(rotatedWireframe.size() == 32,
                "Rotated cylinder should still have 32 wireframe points");
}

void test_cylinderEdgeCases()
{
    std::cout << "\n=== Testing Cylinder Edge Cases ===" << std::endl;

    // Test very small cylinder
    CylinderDimension tinyDim;
    tinyDim.radius_ = 0.01f;
    tinyDim.height_ = 0.02f;

    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CylinderConfig tinyConfig = {originTransform, tinyDim, "TinyCylinder"};

    Cylinder tinyCylinder(tinyConfig);
    auto tinyWireframe = tinyCylinder.wireframe();
    auto tinyMesh = tinyCylinder.surfaceMesh(16);

    assert_true(tinyWireframe.size() == 32, "Tiny cylinder should still have 32 wireframe points");
    assert_true(tinyMesh->getPoints().size() > 0, "Tiny cylinder should still generate mesh points");

    assert_near(tinyCylinder.getRadius(), 0.01f, 0.001f, "Tiny cylinder radius should be preserved");
    assert_near(tinyCylinder.getHeight(), 0.02f, 0.001f, "Tiny cylinder height should be preserved");

    // Test large cylinder
    CylinderDimension largeDim;
    largeDim.radius_ = 50.0f;
    largeDim.height_ = 100.0f;

    CylinderConfig largeConfig = {originTransform, largeDim, "LargeCylinder"};

    Cylinder largeCylinder(largeConfig);
    auto largeWireframe = largeCylinder.wireframe();
    auto largeMesh = largeCylinder.surfaceMesh(32);

    assert_true(largeWireframe.size() == 32, "Large cylinder should still have 32 wireframe points");
    assert_true(largeMesh->getPoints().size() > 0, "Large cylinder should still generate mesh points");

    assert_near(largeCylinder.getRadius(), 50.0f, 0.1f, "Large cylinder radius should be preserved");
    assert_near(largeCylinder.getHeight(), 100.0f, 0.1f, "Large cylinder height should be preserved");

    // Test flat cylinder (very small height)
    CylinderDimension flatDim;
    flatDim.radius_ = 5.0f;
    flatDim.height_ = 0.1f;

    CylinderConfig flatConfig = {originTransform, flatDim, "FlatCylinder"};

    Cylinder flatCylinder(flatConfig);
    auto flatWireframe = flatCylinder.wireframe();
    auto flatMesh = flatCylinder.surfaceMesh(32);

    assert_true(flatWireframe.size() == 32, "Flat cylinder should still have 32 wireframe points");
    assert_true(flatMesh->getPoints().size() > 0, "Flat cylinder should still generate mesh points");
}

void test_cylinderMeshQuality()
{
    std::cout << "\n=== Testing Cylinder Mesh Quality Scaling ===" << std::endl;

    CylinderDimension dim;
    dim.radius_ = 2.0f;
    dim.height_ = 4.0f;

    spatial::Transform originTransform({0, 0, 0}, {0, 0, 0});
    CylinderConfig config = {originTransform, dim, "QualityCylinder"};

    Cylinder cylinder(config);

    // Test various quality levels
    auto mesh8 = cylinder.surfaceMesh(8);
    auto mesh32 = cylinder.surfaceMesh(32);
    auto mesh128 = cylinder.surfaceMesh(128);
    auto mesh512 = cylinder.surfaceMesh(512);

    assert_true(mesh8->getPoints().size() > 0, "Quality 8 mesh should have points");
    assert_true(mesh32->getPoints().size() > 0, "Quality 32 mesh should have points");
    assert_true(mesh128->getPoints().size() > 0, "Quality 128 mesh should have points");
    assert_true(mesh512->getPoints().size() > 0, "Quality 512 mesh should have points");

    // Higher quality should have more points
    assert_true(mesh512->getPoints().size() >= mesh8->getPoints().size(),
                "High quality mesh should have at least as many points as low quality");

    std::cout << "✓ Mesh quality scaling: "
              << "Q8=" << mesh8->getPoints().size() << " points, "
              << "Q32=" << mesh32->getPoints().size() << " points, "
              << "Q128=" << mesh128->getPoints().size() << " points, "
              << "Q512=" << mesh512->getPoints().size() << " points" << std::endl;
}

void test_cylinderGeometricProperties()
{
    std::cout << "\n=== Testing Cylinder Geometric Properties ===" << std::endl;

    // Test cylinder with known dimensions
    CylinderDimension dim;
    dim.radius_ = 3.0f;
    dim.height_ = 8.0f;

    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    CylinderConfig config = {transform, dim, "GeometricCylinder"};

    Cylinder cylinder(config);

    // Test accessor methods
    assert_near(cylinder.getRadius(), 3.0f, 0.01f, "getRadius should return correct value");
    assert_near(cylinder.getHeight(), 8.0f, 0.01f, "getHeight should return correct value");

    // Test mesh generation for coverage of top, bottom, and sides
    auto mesh = cylinder.surfaceMesh(64);

    // Categorize points by height to verify we have top, bottom, and side coverage
    int topPoints = 0, bottomPoints = 0, sidePoints = 0;
    float halfHeight = 4.0f; // height / 2
    float heightTolerance = 0.1f;

    for (const auto &point : mesh->getPoints())
    {
        if (std::abs(point.z() - halfHeight) < heightTolerance)
        {
            topPoints++;
        }
        else if (std::abs(point.z() + halfHeight) < heightTolerance)
        {
            bottomPoints++;
        }
        else if (std::abs(point.z()) < halfHeight - heightTolerance)
        {
            sidePoints++;
        }
    }

    assert_true(topPoints > 0, "Mesh should have points on the top face");
    assert_true(bottomPoints > 0, "Mesh should have points on the bottom face");
    assert_true(sidePoints > 0, "Mesh should have points on the side surface");

    std::cout << "✓ Mesh coverage: Top=" << topPoints
              << ", Bottom=" << bottomPoints
              << ", Sides=" << sidePoints << " points" << std::endl;
}

void test_cylinderToString()
{
    std::cout << "\n=== Testing Cylinder toString Method ===" << std::endl;

    CylinderDimension dim;
    dim.radius_ = 2.75f;
    dim.height_ = 5.5f;

    spatial::Transform transform({1, 2, 3}, {10, 20, 30});
    CylinderConfig config = {transform, dim, "CustomCylinder"};

    Cylinder cylinder(config);
    std::string cylinderStr = cylinder.toString();

    assert_true(cylinderStr.find("Cylinder") != std::string::npos,
                "toString should contain 'Cylinder'");
    assert_true(cylinderStr.find("2.75") != std::string::npos || cylinderStr.find("radius") != std::string::npos,
                "toString should contain radius information");
    assert_true(cylinderStr.find("5.5") != std::string::npos || cylinderStr.find("height") != std::string::npos,
                "toString should contain height information");

    std::cout << "✓ Cylinder toString output: " << cylinderStr << std::endl;
}

int main()
{
    std::cout << "🔶 Starting Cylinder Tests" << std::endl;
    std::cout << "===========================" << std::endl;

    try
    {
        test_cylinderConfigStructure();
        test_cylinderConstruction();
        test_cylinderWireframe();
        test_cylinderSurfaceMesh();
        test_cylinderTransforms();
        test_cylinderEdgeCases();
        test_cylinderMeshQuality();
        test_cylinderGeometricProperties();
        test_cylinderToString();

        std::cout << "\n🎉 All Cylinder tests passed!" << std::endl;
        std::cout << "=============================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Cylinder test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Cylinder test failed with unknown exception" << std::endl;
        return 1;
    }
}
