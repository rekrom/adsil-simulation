#include "utils/JsonFileUtils.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cassert>

// Simple test assertion helper
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

// Test helper to create temporary files
class TempFileHelper
{
private:
    std::string tempDir;
    std::vector<std::string> createdFiles;

public:
    TempFileHelper()
    {
        tempDir = std::filesystem::temp_directory_path() / "utils_test";
        std::filesystem::create_directories(tempDir);
    }

    ~TempFileHelper()
    {
        cleanup();
    }

    std::string createTempFile(const std::string &content, const std::string &suffix = ".json")
    {
        static int counter = 0;
        std::string filename = tempDir + "/test_" + std::to_string(counter++) + suffix;

        std::ofstream file(filename);
        file << content;
        file.close();

        createdFiles.push_back(filename);
        return filename;
    }

    std::string getTempPath(const std::string &filename)
    {
        return tempDir + "/" + filename;
    }

    void cleanup()
    {
        for (const auto &file : createdFiles)
        {
            if (std::filesystem::exists(file))
            {
                std::filesystem::remove(file);
            }
        }
        if (std::filesystem::exists(tempDir))
        {
            std::filesystem::remove_all(tempDir);
        }
    }
};

// Test Functions

void test_readJsonFromFile_validFile()
{
    std::cout << "\n=== Testing readJsonFromFile with valid JSON ===" << std::endl;

    TempFileHelper helper;

    // Test simple JSON object
    std::string jsonContent = R"({"name": "test", "value": 42, "active": true})";
    std::string filename = helper.createTempFile(jsonContent);

    nlohmann::json result;
    bool success = utils::readJsonFromFile(filename, result);

    assert_true(success, "readJsonFromFile should succeed with valid JSON");
    assert_true(result["name"] == "test", "Should parse string value correctly");
    assert_true(result["value"] == 42, "Should parse integer value correctly");
    assert_true(result["active"] == true, "Should parse boolean value correctly");
}

void test_readJsonFromFile_validArray()
{
    std::cout << "\n=== Testing readJsonFromFile with JSON array ===" << std::endl;

    TempFileHelper helper;

    std::string jsonContent = R"([1, 2, 3, "test", {"nested": true}])";
    std::string filename = helper.createTempFile(jsonContent);

    nlohmann::json result;
    bool success = utils::readJsonFromFile(filename, result);

    assert_true(success, "readJsonFromFile should succeed with valid JSON array");
    assert_true(result.is_array(), "Result should be an array");
    assert_true(result.size() == 5, "Array should have 5 elements");
    assert_true(result[0] == 1, "First element should be 1");
    assert_true(result[3] == "test", "Fourth element should be 'test'");
    assert_true(result[4]["nested"] == true, "Nested object should be parsed correctly");
}

void test_readJsonFromFile_nonexistentFile()
{
    std::cout << "\n=== Testing readJsonFromFile with nonexistent file ===" << std::endl;

    nlohmann::json result;
    bool success = utils::readJsonFromFile("/nonexistent/path/file.json", result);

    assert_false(success, "readJsonFromFile should fail with nonexistent file");
}

void test_readJsonFromFile_invalidJson()
{
    std::cout << "\n=== Testing readJsonFromFile with invalid JSON ===" << std::endl;

    TempFileHelper helper;

    std::string invalidJson = R"({"name": "test", "value":})"; // Missing value
    std::string filename = helper.createTempFile(invalidJson);

    nlohmann::json result;
    bool success = utils::readJsonFromFile(filename, result);

    assert_false(success, "readJsonFromFile should fail with invalid JSON");
}

void test_readJsonFromFile_emptyFile()
{
    std::cout << "\n=== Testing readJsonFromFile with empty file ===" << std::endl;

    TempFileHelper helper;

    std::string filename = helper.createTempFile("");

    nlohmann::json result;
    bool success = utils::readJsonFromFile(filename, result);

    assert_false(success, "readJsonFromFile should fail with empty file");
}

void test_writeJsonToFile_simpleObject()
{
    std::cout << "\n=== Testing writeJsonToFile with simple object ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json testJson = {
        {"name", "test"},
        {"value", 42},
        {"active", true},
        {"data", nlohmann::json::array({1, 2, 3})}};

    std::string filename = helper.getTempPath("write_test.json");
    bool success = utils::writeJsonToFile(filename, testJson);

    assert_true(success, "writeJsonToFile should succeed");
    assert_true(std::filesystem::exists(filename), "File should be created");

    // Verify the written content by reading it back
    nlohmann::json readBack;
    bool readSuccess = utils::readJsonFromFile(filename, readBack);

    assert_true(readSuccess, "Should be able to read back written file");
    assert_true(readBack == testJson, "Read content should match written content");
}

void test_writeJsonToFile_complexObject()
{
    std::cout << "\n=== Testing writeJsonToFile with complex object ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json complexJson = {
        {"metadata", {{"version", "1.0"}, {"author", "test_user"}, {"timestamp", "2024-01-01T00:00:00Z"}}},
        {"data", {{"points", nlohmann::json::array({{{"x", 1.0}, {"y", 2.0}, {"z", 3.0}}, {{"x", 4.0}, {"y", 5.0}, {"z", 6.0}}})}, {"settings", {{"enabled", true}, {"threshold", 0.5}, {"modes", nlohmann::json::array({"normal", "debug", "performance"})}}}}}};

    std::string filename = helper.getTempPath("complex_test.json");
    bool success = utils::writeJsonToFile(filename, complexJson);

    assert_true(success, "writeJsonToFile should succeed with complex object");

    // Verify by reading back
    nlohmann::json readBack;
    bool readSuccess = utils::readJsonFromFile(filename, readBack);

    assert_true(readSuccess, "Should be able to read back complex file");
    assert_true(readBack == complexJson, "Complex content should match");
}

void test_writeJsonToFile_invalidPath()
{
    std::cout << "\n=== Testing writeJsonToFile with invalid path ===" << std::endl;

    nlohmann::json testJson = {{"test", "value"}};

    // Try to write to a path that doesn't exist and can't be created
    bool success = utils::writeJsonToFile("/root/nonexistent/path/file.json", testJson);

    assert_false(success, "writeJsonToFile should fail with invalid path");
}

void test_loadJsonOrExit_validFile()
{
    std::cout << "\n=== Testing loadJsonOrExit with valid file ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json testJson = {
        {"config", {{"name", "test_config"}, {"version", 2}, {"features", nlohmann::json::array({"feature1", "feature2"})}}}};

    std::string filename = helper.createTempFile(testJson.dump(4));

    try
    {
        nlohmann::json result = utils::loadJsonOrExit(filename);
        assert_true(result == testJson, "loadJsonOrExit should return correct JSON");
        assert_true(result["config"]["name"] == "test_config", "Should access nested values correctly");
    }
    catch (const std::exception &e)
    {
        assert_true(false, "loadJsonOrExit should not throw with valid file");
    }
}

void test_loadJsonOrExit_invalidFile()
{
    std::cout << "\n=== Testing loadJsonOrExit with invalid file ===" << std::endl;

    try
    {
        nlohmann::json result = utils::loadJsonOrExit("/nonexistent/file.json");
        assert_true(false, "loadJsonOrExit should throw with invalid file");
    }
    catch (const std::runtime_error &e)
    {
        std::string errorMsg = e.what();
        assert_true(errorMsg.find("Failed to read JSON file") != std::string::npos,
                    "Exception should contain proper error message");
        std::cout << "✓ loadJsonOrExit correctly threw exception: " << errorMsg << std::endl;
    }
    catch (...)
    {
        assert_true(false, "loadJsonOrExit should throw runtime_error specifically");
    }
}

void test_roundTripJsonFiles()
{
    std::cout << "\n=== Testing round-trip JSON file operations ===" << std::endl;

    TempFileHelper helper;

    // Create multiple test JSONs with different structures
    std::vector<nlohmann::json> testCases = {
        // Simple object
        {{"simple", "test"}},

        // Array
        nlohmann::json::array({1, 2, 3, "test", true}),

        // Complex nested structure
        {
            {"level1", {{"level2", {{"level3", {{"deep_value", "found"}, {"deep_array", nlohmann::json::array({1, 2, 3})}, {"deep_bool", false}}}}}}},
            {"numbers", {{"int", 42}, {"float", 3.14159}, {"negative", -123}}},
            {"special_chars", "Special: äöü ñ 中文 🚀"}},

        // Empty structures
        nlohmann::json::object(),
        nlohmann::json::array()};

    for (size_t i = 0; i < testCases.size(); ++i)
    {
        std::string filename = helper.getTempPath("roundtrip_" + std::to_string(i) + ".json");

        // Write
        bool writeSuccess = utils::writeJsonToFile(filename, testCases[i]);
        assert_true(writeSuccess, "Write should succeed for test case " + std::to_string(i));

        // Read back
        nlohmann::json readResult;
        bool readSuccess = utils::readJsonFromFile(filename, readResult);
        assert_true(readSuccess, "Read should succeed for test case " + std::to_string(i));

        // Compare
        assert_true(readResult == testCases[i], "Round-trip should preserve data for test case " + std::to_string(i));
    }
}

void test_jsonFormatting()
{
    std::cout << "\n=== Testing JSON formatting ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json testJson = {
        {"name", "format_test"},
        {"nested", {{"array", nlohmann::json::array({1, 2, 3})}, {"bool", true}}}};

    std::string filename = helper.getTempPath("format_test.json");
    bool success = utils::writeJsonToFile(filename, testJson);
    assert_true(success, "Should write formatted JSON successfully");

    // Read the raw file content to check formatting
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    // Check that the file contains proper indentation (writeJsonToFile uses dump(4))
    assert_true(content.find("    ") != std::string::npos, "File should contain 4-space indentation");
    assert_true(content.find("\n") != std::string::npos, "File should contain newlines");

    std::cout << "✓ JSON file is properly formatted with indentation" << std::endl;
}

int main()
{
    std::cout << "🧪 Starting Utils Module Tests" << std::endl;
    std::cout << "================================" << std::endl;

    try
    {
        // Test readJsonFromFile
        test_readJsonFromFile_validFile();
        test_readJsonFromFile_validArray();
        test_readJsonFromFile_nonexistentFile();
        test_readJsonFromFile_invalidJson();
        test_readJsonFromFile_emptyFile();

        // Test writeJsonToFile
        test_writeJsonToFile_simpleObject();
        test_writeJsonToFile_complexObject();
        test_writeJsonToFile_invalidPath();

        // Test loadJsonOrExit
        test_loadJsonOrExit_validFile();
        test_loadJsonOrExit_invalidFile();

        // Integration tests
        test_roundTripJsonFiles();
        test_jsonFormatting();

        std::cout << "\n🎉 All Utils tests passed successfully!" << std::endl;
        std::cout << "================================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
