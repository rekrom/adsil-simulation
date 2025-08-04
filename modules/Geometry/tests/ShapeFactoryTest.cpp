#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <spatial/implementations/Transform.hpp>
#include <iostream>
#include <cassert>
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

void test_shapeFactoryCreateCube()
{
    std::cout << "\n=== Testing ShapeFactory Cube Creation ===" << std::endl;

    // Create cube configuration
    CubeDimension cubeDim(5.0f);
    spatial::Transform cubeTransform({1, 2, 3}, {10, 20, 30});
    CubeConfig cubeConfig = {cubeTransform, cubeDim, "FactoryCube"};

    // Create cube via factory
    auto cube = ShapeFactory::createCube(cubeConfig);

    assert_true(cube != nullptr, "Factory should create a valid cube");
    assert_true(std::dynamic_pointer_cast<Cube>(cube) != nullptr,
                "Factory should return a Cube object");

    // Test that the cube has correct properties
    auto cubePtr = std::dynamic_pointer_cast<Cube>(cube);
    auto dimensions = cubePtr->getDimension();

    assert_near(dimensions.length, 5.0f, 0.01f, "Factory cube should have correct length");
    assert_near(dimensions.width, 5.0f, 0.01f, "Factory cube should have correct width");
    assert_near(dimensions.height, 5.0f, 0.01f, "Factory cube should have correct height");

    // Test cube functionality
    auto wireframe = cube->wireframe();
    assert_true(wireframe.size() == 24, "Factory cube should generate correct wireframe");

    auto mesh = cube->surfaceMesh(64);
    assert_true(mesh != nullptr, "Factory cube should generate surface mesh");
    assert_true(mesh->getPoints().size() > 0, "Factory cube mesh should have points");

    std::string cubeStr = cube->toString();
    assert_true(cubeStr.find("Cube") != std::string::npos,
                "Factory cube toString should identify as Cube");
}

void test_shapeFactoryCreateCylinder()
{
    std::cout << "\n=== Testing ShapeFactory Cylinder Creation ===" << std::endl;

    // Create cylinder configuration
    CylinderDimension cylinderDim(8.0f, 3.0f); // height, radius
    spatial::Transform cylinderTransform({-2, 4, -1}, {45, 0, 90});
    CylinderConfig cylinderConfig = {cylinderTransform, cylinderDim, "FactoryCylinder"};

    // Create cylinder via factory
    auto cylinder = ShapeFactory::createCylinder(cylinderConfig);

    assert_true(cylinder != nullptr, "Factory should create a valid cylinder");
    assert_true(std::dynamic_pointer_cast<Cylinder>(cylinder) != nullptr,
                "Factory should return a Cylinder object");

    // Test that the cylinder has correct properties
    auto cylinderPtr = std::dynamic_pointer_cast<Cylinder>(cylinder);

    assert_near(cylinderPtr->getHeight(), 8.0f, 0.01f, "Factory cylinder should have correct height");
    assert_near(cylinderPtr->getRadius(), 3.0f, 0.01f, "Factory cylinder should have correct radius");

    // Test cylinder functionality
    auto wireframe = cylinder->wireframe();
    assert_true(wireframe.size() == 32, "Factory cylinder should generate correct wireframe");

    auto mesh = cylinder->surfaceMesh(128);
    assert_true(mesh != nullptr, "Factory cylinder should generate surface mesh");
    assert_true(mesh->getPoints().size() > 0, "Factory cylinder mesh should have points");

    std::string cylinderStr = cylinder->toString();
    assert_true(cylinderStr.find("Cylinder") != std::string::npos,
                "Factory cylinder toString should identify as Cylinder");
}

void test_shapeFactoryMultipleCreations()
{
    std::cout << "\n=== Testing ShapeFactory Multiple Creations ===" << std::endl;

    // Create multiple cubes
    std::vector<std::shared_ptr<ShapeBase>> cubes;

    for (int i = 0; i < 5; ++i)
    {
        float fi = static_cast<float>(i);
        CubeDimension dim(1.0f + fi);
        spatial::Transform transform({fi, fi * 2.0f, fi * 3.0f}, {fi * 10.0f, 0, 0});
        CubeConfig config = {transform, dim, "Cube" + std::to_string(i)};

        cubes.push_back(ShapeFactory::createCube(config));
    }

    assert_true(cubes.size() == 5, "Should create 5 cubes");

    for (size_t i = 0; i < cubes.size(); ++i)
    {
        assert_true(cubes[i] != nullptr, "Each cube should be valid");
        assert_true(std::dynamic_pointer_cast<Cube>(cubes[i]) != nullptr,
                    "Each object should be a Cube");

        auto cubePtr = std::dynamic_pointer_cast<Cube>(cubes[i]);
        auto dimensions = cubePtr->getDimension();
        float expectedDim = 1.0f + static_cast<float>(i);
        assert_near(dimensions.length, expectedDim, 0.01f,
                    "Cube " + std::to_string(i) + " should have correct dimensions");
    }

    // Create multiple cylinders
    std::vector<std::shared_ptr<ShapeBase>> cylinders;

    for (int i = 0; i < 3; ++i)
    {
        float fi = static_cast<float>(i);
        CylinderDimension dim(2.0f + fi, 1.0f + fi * 0.5f); // height, radius
        spatial::Transform transform({fi * 5.0f, 0, 0}, {0, fi * 15.0f, 0});
        CylinderConfig config = {transform, dim, "Cylinder" + std::to_string(i)};

        cylinders.push_back(ShapeFactory::createCylinder(config));
    }

    assert_true(cylinders.size() == 3, "Should create 3 cylinders");

    for (size_t i = 0; i < cylinders.size(); ++i)
    {
        assert_true(cylinders[i] != nullptr, "Each cylinder should be valid");
        assert_true(std::dynamic_pointer_cast<Cylinder>(cylinders[i]) != nullptr,
                    "Each object should be a Cylinder");

        auto cylinderPtr = std::dynamic_pointer_cast<Cylinder>(cylinders[i]);
        float expectedHeight = 2.0f + static_cast<float>(i);
        float expectedRadius = 1.0f + static_cast<float>(i) * 0.5f;

        assert_near(cylinderPtr->getHeight(), expectedHeight, 0.01f,
                    "Cylinder " + std::to_string(i) + " should have correct height");
        assert_near(cylinderPtr->getRadius(), expectedRadius, 0.01f,
                    "Cylinder " + std::to_string(i) + " should have correct radius");
    }
}

void test_shapeFactoryEdgeCases()
{
    std::cout << "\n=== Testing ShapeFactory Edge Cases ===" << std::endl;

    // Test cube with zero dimensions
    CubeDimension zeroDim(0.0f);
    spatial::Transform zeroTransform({0, 0, 0}, {0, 0, 0});
    CubeConfig zeroConfig = {zeroTransform, zeroDim, "ZeroCube"};

    auto zeroCube = ShapeFactory::createCube(zeroConfig);
    assert_true(zeroCube != nullptr, "Factory should handle zero-dimension cube");

    auto zeroCubePtr = std::dynamic_pointer_cast<Cube>(zeroCube);
    auto zeroDimensions = zeroCubePtr->getDimension();
    assert_near(zeroDimensions.length, 0.0f, 0.01f, "Zero cube should preserve zero dimensions");

    // Test cylinder with very small dimensions
    CylinderDimension tinyDim(0.001f, 0.001f);
    CylinderConfig tinyConfig = {zeroTransform, tinyDim, "TinyCylinder"};

    auto tinyCylinder = ShapeFactory::createCylinder(tinyConfig);
    assert_true(tinyCylinder != nullptr, "Factory should handle tiny cylinder");

    auto tinyCylinderPtr = std::dynamic_pointer_cast<Cylinder>(tinyCylinder);
    assert_near(tinyCylinderPtr->getHeight(), 0.001f, 0.0001f, "Tiny cylinder should preserve small height");
    assert_near(tinyCylinderPtr->getRadius(), 0.001f, 0.0001f, "Tiny cylinder should preserve small radius");

    // Test cube with very large dimensions
    CubeDimension largeDim(1000.0f);
    CubeConfig largeConfig = {zeroTransform, largeDim, "LargeCube"};

    auto largeCube = ShapeFactory::createCube(largeConfig);
    assert_true(largeCube != nullptr, "Factory should handle large cube");

    auto largeCubePtr = std::dynamic_pointer_cast<Cube>(largeCube);
    auto largeDimensions = largeCubePtr->getDimension();
    assert_near(largeDimensions.length, 1000.0f, 0.1f, "Large cube should preserve large dimensions");

    // Test cylinder with extreme aspect ratio (very tall and thin)
    CylinderDimension extremeDim(100.0f, 0.1f); // height, radius
    CylinderConfig extremeConfig = {zeroTransform, extremeDim, "ExtremeCylinder"};

    auto extremeCylinder = ShapeFactory::createCylinder(extremeConfig);
    assert_true(extremeCylinder != nullptr, "Factory should handle extreme aspect ratio cylinder");

    auto extremeCylinderPtr = std::dynamic_pointer_cast<Cylinder>(extremeCylinder);
    assert_near(extremeCylinderPtr->getHeight(), 100.0f, 0.1f, "Extreme cylinder should preserve large height");
    assert_near(extremeCylinderPtr->getRadius(), 0.1f, 0.01f, "Extreme cylinder should preserve small radius");
}

void test_shapeFactoryComplexTransforms()
{
    std::cout << "\n=== Testing ShapeFactory with Complex Transforms ===" << std::endl;

    // Test cube with complex transform
    CubeDimension cubeDim(2.0f);
    spatial::Transform complexTransform({10.5f, -20.3f, 5.7f}, {45.0f, 90.0f, 135.0f});
    CubeConfig complexConfig = {complexTransform, cubeDim, "ComplexCube"};

    auto complexCube = ShapeFactory::createCube(complexConfig);
    assert_true(complexCube != nullptr, "Factory should handle complex transform cube");

    // Test that the cube still functions correctly
    auto wireframe = complexCube->wireframe();
    assert_true(wireframe.size() == 24, "Complex transform cube should generate correct wireframe");

    auto mesh = complexCube->surfaceMesh(32);
    assert_true(mesh != nullptr && mesh->getPoints().size() > 0,
                "Complex transform cube should generate mesh");

    // Test cylinder with complex transform
    CylinderDimension cylinderDim(4.0f, 1.5f);
    CylinderConfig complexCylinderConfig = {complexTransform, cylinderDim, "ComplexCylinder"};

    auto complexCylinder = ShapeFactory::createCylinder(complexCylinderConfig);
    assert_true(complexCylinder != nullptr, "Factory should handle complex transform cylinder");

    // Test that the cylinder still functions correctly
    auto cylinderWireframe = complexCylinder->wireframe();
    assert_true(cylinderWireframe.size() == 32, "Complex transform cylinder should generate correct wireframe");

    auto cylinderMesh = complexCylinder->surfaceMesh(64);
    assert_true(cylinderMesh != nullptr && cylinderMesh->getPoints().size() > 0,
                "Complex transform cylinder should generate mesh");
}

void test_shapeFactoryPolymorphism()
{
    std::cout << "\n=== Testing ShapeFactory Polymorphism ===" << std::endl;

    // Create shapes and store them as base class pointers
    std::vector<std::shared_ptr<ShapeBase>> shapes;

    // Add cubes
    for (int i = 0; i < 3; ++i)
    {
        float fi = static_cast<float>(i);
        CubeDimension dim(1.0f + fi);
        spatial::Transform transform({fi, 0, 0}, {0, 0, 0});
        CubeConfig config = {transform, dim, "PolyCube" + std::to_string(fi)};
        shapes.push_back(ShapeFactory::createCube(config));
    }

    // Add cylinders
    for (int i = 0; i < 2; ++i)
    {
        float fi = static_cast<float>(i);
        CylinderDimension dim(2.0f + fi, 1.0f + fi);
        spatial::Transform transform({fi + 10, 0, 0}, {0, 0, 0});
        CylinderConfig config = {transform, dim, "PolyCylinder" + std::to_string(fi)};
        shapes.push_back(ShapeFactory::createCylinder(config));
    }

    assert_true(shapes.size() == 5, "Should have 5 shapes total");

    // Test polymorphic behavior
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        assert_true(shapes[i] != nullptr, "Each shape should be valid");

        // Test polymorphic method calls
        auto wireframe = shapes[i]->wireframe();
        assert_true(wireframe.size() > 0, "Each shape should generate wireframe");

        auto mesh = shapes[i]->surfaceMesh(32);
        assert_true(mesh != nullptr, "Each shape should generate mesh");
        assert_true(mesh->getPoints().size() > 0, "Each mesh should have points");

        std::string shapeStr = shapes[i]->toString();
        assert_true(!shapeStr.empty(), "Each shape should have non-empty toString");

        // Verify shape types
        if (i < 3)
        {
            assert_true(wireframe.size() == 24, "Cube wireframes should have 24 points");
            assert_true(shapeStr.find("Cube") != std::string::npos,
                        "Cube toString should contain 'Cube'");
        }
        else
        {
            assert_true(wireframe.size() == 32, "Cylinder wireframes should have 32 points");
            assert_true(shapeStr.find("Cylinder") != std::string::npos,
                        "Cylinder toString should contain 'Cylinder'");
        }
    }
}

void test_shapeFactoryMemoryManagement()
{
    std::cout << "\n=== Testing ShapeFactory Memory Management ===" << std::endl;

    // Test that factory returns proper shared_ptr objects
    CubeDimension cubeDim(1.0f);
    spatial::Transform transform({0, 0, 0}, {0, 0, 0});
    CubeConfig cubeConfig = {transform, cubeDim, "MemoryCube"};

    auto cube1 = ShapeFactory::createCube(cubeConfig);
    auto cube2 = cube1; // Copy shared_ptr

    assert_true(cube1.get() == cube2.get(), "Shared pointers should point to same object");
    assert_true(cube1.use_count() == 2, "Reference count should be 2");

    {
        auto cube3 = cube1; // Create another reference in a scope
        assert_true(cube1.use_count() == 3, "Reference count should be 3 in inner scope");
    }

    assert_true(cube1.use_count() == 2, "Reference count should be back to 2 after scope exit");

    // Test factory creates independent objects
    auto anotherCube = ShapeFactory::createCube(cubeConfig);
    assert_true(cube1.get() != anotherCube.get(), "Factory should create independent objects");
    assert_true(cube1.use_count() == 2, "Original cube reference count should remain 2");
    assert_true(anotherCube.use_count() == 1, "New cube should have reference count 1");
}

int main()
{
    std::cout << "🏭 Starting ShapeFactory Tests" << std::endl;
    std::cout << "===============================" << std::endl;

    try
    {
        test_shapeFactoryCreateCube();
        test_shapeFactoryCreateCylinder();
        test_shapeFactoryMultipleCreations();
        test_shapeFactoryEdgeCases();
        test_shapeFactoryComplexTransforms();
        test_shapeFactoryPolymorphism();
        test_shapeFactoryMemoryManagement();

        std::cout << "\n🎉 All ShapeFactory tests passed!" << std::endl;
        std::cout << "==================================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ ShapeFactory test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ ShapeFactory test failed with unknown exception" << std::endl;
        return 1;
    }
}
