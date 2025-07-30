#include <simulation/configs/SimulationConfig.hpp>
#include <iostream>
#include <cassert>
#include <cstdlib>

// Simple test framework
class SimpleTest
{
public:
    static void assert_true(bool condition, const std::string &message)
    {
        if (!condition)
        {
            std::cerr << "[FAIL] " << message << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_int(int expected, int actual, const std::string &message)
    {
        if (expected != actual)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_float(float expected, float actual, const std::string &message, float tolerance = 0.001f)
    {
        if (std::abs(expected - actual) > tolerance)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }

    static void assert_equal_string(const std::string &expected, const std::string &actual, const std::string &message)
    {
        if (expected != actual)
        {
            std::cerr << "[FAIL] " << message << " - Expected: " << expected << ", Actual: " << actual << std::endl;
            exit(1);
        }
        else
        {
            std::cout << "[PASS] " << message << std::endl;
        }
    }
};

void testDefaultConfigCreation()
{
    std::cout << "\n=== Testing Default Configuration Creation ===" << std::endl;

    // Clear environment variable for clean testing
    unsetenv("ADSIL_RESOURCE_PATH");

    auto config = simulation::SimulationConfig::createDefault();

    SimpleTest::assert_true(config != nullptr, "Configuration object created");

    // Test default window configuration
    const auto &windowConfig = config->getWindowConfig();
    SimpleTest::assert_equal_int(1280, windowConfig.width, "Default window width");
    SimpleTest::assert_equal_int(720, windowConfig.height, "Default window height");
    SimpleTest::assert_equal_string("ADSIL Analyzer - OpenGL", windowConfig.title, "Default window title");

    // Test default frame configuration
    const auto &frameConfig = config->getFrameConfig();
    SimpleTest::assert_equal_int(3, frameConfig.bufferWindowSize, "Default frame buffer window size");

    // Test default point cloud configuration
    const auto &pointCloudConfig = config->getPointCloudConfig();
    SimpleTest::assert_equal_float(2.0f, pointCloudConfig.pointSize, "Default point cloud size");
    SimpleTest::assert_equal_float(1.0f, pointCloudConfig.alpha, "Default point cloud alpha");
    SimpleTest::assert_equal_string("PointCloudEntity - Outside Point Cloud", pointCloudConfig.outsideName, "Default outside point cloud name");
    SimpleTest::assert_equal_string("PointCloudEntity - Selected Point Cloud", pointCloudConfig.detectedName, "Default detected point cloud name");

    // Test default resource configuration
    const auto &resourceConfig = config->getResourceConfig();
    SimpleTest::assert_equal_string("/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources", resourceConfig.basePath, "Default resource base path");
    SimpleTest::assert_equal_string("scene.json", resourceConfig.sceneFile, "Default scene file name");
}

void testEnvironmentVariableOverride()
{
    std::cout << "\n=== Testing Environment Variable Override ===" << std::endl;

    // Set environment variable
    setenv("ADSIL_RESOURCE_PATH", "/custom/test/path", 1);

    auto config = simulation::SimulationConfig::createDefault();

    SimpleTest::assert_true(config != nullptr, "Configuration object created with env var");

    // Check that environment variable was used
    const auto &resourceConfig = config->getResourceConfig();
    SimpleTest::assert_equal_string("/custom/test/path", resourceConfig.basePath, "Environment variable override");
    SimpleTest::assert_equal_string("scene.json", resourceConfig.sceneFile, "Scene file remains default");

    // Clean up
    unsetenv("ADSIL_RESOURCE_PATH");
}

void testConfigurationSetters()
{
    std::cout << "\n=== Testing Configuration Setters ===" << std::endl;

    auto config = simulation::SimulationConfig::createDefault();

    // Test window configuration setter
    simulation::SimulationConfig::WindowConfig newWindowConfig;
    newWindowConfig.width = 1920;
    newWindowConfig.height = 1080;
    newWindowConfig.title = "Custom Title";

    config->setWindowConfig(newWindowConfig);

    const auto &updatedWindowConfig = config->getWindowConfig();
    SimpleTest::assert_equal_int(1920, updatedWindowConfig.width, "Updated window width");
    SimpleTest::assert_equal_int(1080, updatedWindowConfig.height, "Updated window height");
    SimpleTest::assert_equal_string("Custom Title", updatedWindowConfig.title, "Updated window title");

    // Test frame configuration setter
    simulation::SimulationConfig::FrameConfig newFrameConfig;
    newFrameConfig.bufferWindowSize = 5;

    config->setFrameConfig(newFrameConfig);

    const auto &updatedFrameConfig = config->getFrameConfig();
    SimpleTest::assert_equal_int(5, updatedFrameConfig.bufferWindowSize, "Updated frame buffer size");

    // Test point cloud configuration setter
    simulation::SimulationConfig::PointCloudConfig newPointCloudConfig;
    newPointCloudConfig.pointSize = 3.5f;
    newPointCloudConfig.alpha = 0.8f;
    newPointCloudConfig.detectedColor = glm::vec3(1.0f, 0.0f, 0.0f);
    newPointCloudConfig.outsideName = "Custom Outside";
    newPointCloudConfig.detectedName = "Custom Detected";

    config->setPointCloudConfig(newPointCloudConfig);

    const auto &updatedPointCloudConfig = config->getPointCloudConfig();
    SimpleTest::assert_equal_float(3.5f, updatedPointCloudConfig.pointSize, "Updated point cloud size");
    SimpleTest::assert_equal_float(0.8f, updatedPointCloudConfig.alpha, "Updated point cloud alpha");
    SimpleTest::assert_equal_string("Custom Outside", updatedPointCloudConfig.outsideName, "Updated outside name");
    SimpleTest::assert_equal_string("Custom Detected", updatedPointCloudConfig.detectedName, "Updated detected name");

    // Test car configuration setter
    simulation::SimulationConfig::CarConfig newCarConfig;
    newCarConfig.color = glm::vec3(1.0f, 1.0f, 0.0f);

    config->setCarConfig(newCarConfig);

    const auto &updatedCarConfig = config->getCarConfig();
    SimpleTest::assert_equal_float(1.0f, updatedCarConfig.color.r, "Updated car color red");
    SimpleTest::assert_equal_float(1.0f, updatedCarConfig.color.g, "Updated car color green");
    SimpleTest::assert_equal_float(0.0f, updatedCarConfig.color.b, "Updated car color blue");

    // Test resource configuration setter
    simulation::SimulationConfig::ResourceConfig newResourceConfig;
    newResourceConfig.basePath = "/new/resource/path";
    newResourceConfig.sceneFile = "custom_scene.json";

    config->setResourceConfig(newResourceConfig);

    const auto &updatedResourceConfig = config->getResourceConfig();
    SimpleTest::assert_equal_string("/new/resource/path", updatedResourceConfig.basePath, "Updated resource base path");
    SimpleTest::assert_equal_string("custom_scene.json", updatedResourceConfig.sceneFile, "Updated scene file name");
}

int main()
{
    std::cout << "SimulationConfig Test Suite" << std::endl;
    std::cout << "===========================" << std::endl;

    try
    {
        testDefaultConfigCreation();
        testEnvironmentVariableOverride();
        testConfigurationSetters();

        std::cout << "\n=== All Tests Passed ===" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
