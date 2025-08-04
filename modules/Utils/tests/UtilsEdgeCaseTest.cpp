#include "utils/JsonFileUtils.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>

// Performance and edge case tests for Utils module

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

class TempFileHelper
{
private:
    std::string tempDir;
    std::vector<std::string> createdFiles;

public:
    TempFileHelper()
    {
        tempDir = std::filesystem::temp_directory_path() / "utils_edge_test";
        std::filesystem::create_directories(tempDir);
    }

    ~TempFileHelper()
    {
        cleanup();
    }

    std::string createTempFile(const std::string &content, const std::string &suffix = ".json")
    {
        static int counter = 0;
        std::string filename = tempDir + "/edge_test_" + std::to_string(counter++) + suffix;

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

void test_largeJsonFile()
{
    std::cout << "\n=== Testing large JSON file handling ===" << std::endl;

    TempFileHelper helper;

    // Create a large JSON with many entries
    nlohmann::json largeJson = nlohmann::json::object();

    // Add 1000 entries with various data types
    for (int i = 0; i < 1000; ++i)
    {
        std::string key = "item_" + std::to_string(i);
        largeJson[key] = {
            {"id", i},
            {"name", "Test Item " + std::to_string(i)},
            {"active", (i % 2 == 0)},
            {"value", i * 3.14159},
            {"tags", nlohmann::json::array({"tag1", "tag2", "tag_" + std::to_string(i % 10)})},
            {"metadata", {{"created", "2024-01-01T00:00:00Z"}, {"updated", "2024-01-02T00:00:00Z"}, {"version", i % 100}}}};
    }

    std::string filename = helper.getTempPath("large_test.json");

    // Measure write performance
    auto start = std::chrono::high_resolution_clock::now();
    bool writeSuccess = utils::writeJsonToFile(filename, largeJson);
    auto writeEnd = std::chrono::high_resolution_clock::now();

    assert_true(writeSuccess, "Should successfully write large JSON file");

    // Measure read performance
    nlohmann::json readResult;
    auto readStart = std::chrono::high_resolution_clock::now();
    bool readSuccess = utils::readJsonFromFile(filename, readResult);
    auto readEnd = std::chrono::high_resolution_clock::now();

    assert_true(readSuccess, "Should successfully read large JSON file");
    assert_true(readResult == largeJson, "Large JSON should round-trip correctly");
    assert_true(readResult.size() == 1000, "Should have correct number of entries");

    auto writeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(writeEnd - start);
    auto readDuration = std::chrono::duration_cast<std::chrono::milliseconds>(readEnd - readStart);

    std::cout << "✓ Write time: " << writeDuration.count() << "ms" << std::endl;
    std::cout << "✓ Read time: " << readDuration.count() << "ms" << std::endl;

    // Verify file size
    auto fileSize = std::filesystem::file_size(filename);
    std::cout << "✓ File size: " << fileSize << " bytes" << std::endl;
    assert_true(fileSize > 100000, "Large JSON should produce substantial file size");
}

void test_unicodeAndSpecialCharacters()
{
    std::cout << "\n=== Testing Unicode and special characters ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json unicodeJson = {
        {"english", "Hello World"},
        {"german", "Hallo Welt - äöüß"},
        {"spanish", "Hola Mundo - ñáéíóú"},
        {"chinese", "你好世界"},
        {"japanese", "こんにちは世界"},
        {"russian", "Привет мир"},
        {"arabic", "مرحبا بالعالم"},
        {"emoji", "🌍🚀🎉💻📊"},
        {"special_chars", "!@#$%^&*()_+-=[]{}|;':\",./<>?"},
        {"quotes", R"(Contains "quotes" and 'apostrophes')"},
        {"newlines", "Line 1\nLine 2\nLine 3"},
        {"tabs", "Tab\tSeparated\tValues"},
        {"backslashes", "Path\\to\\file\\with\\backslashes"},
        {"null_char", std::string("Before\0After", 12)} // String with null character
    };

    std::string filename = helper.getTempPath("unicode_test.json");

    bool writeSuccess = utils::writeJsonToFile(filename, unicodeJson);
    assert_true(writeSuccess, "Should write Unicode JSON successfully");

    nlohmann::json readResult;
    bool readSuccess = utils::readJsonFromFile(filename, readResult);
    assert_true(readSuccess, "Should read Unicode JSON successfully");

    // Verify specific Unicode content
    assert_true(readResult["german"] == "Hallo Welt - äöüß", "German umlauts should be preserved");
    assert_true(readResult["chinese"] == "你好世界", "Chinese characters should be preserved");
    assert_true(readResult["emoji"] == "🌍🚀🎉💻📊", "Emojis should be preserved");
    assert_true(readResult["special_chars"] == "!@#$%^&*()_+-=[]{}|;':\",./<>?", "Special characters should be preserved");

    std::cout << "✓ All Unicode and special characters preserved correctly" << std::endl;
}

void test_edgeCaseJsonStructures()
{
    std::cout << "\n=== Testing edge case JSON structures ===" << std::endl;

    TempFileHelper helper;

    // Test deeply nested structure
    nlohmann::json deeplyNested = nlohmann::json::object();
    nlohmann::json *current = &deeplyNested;

    for (int i = 0; i < 100; ++i)
    {
        (*current)["level_" + std::to_string(i)] = nlohmann::json::object();
        current = &((*current)["level_" + std::to_string(i)]);
    }
    (*current)["final_value"] = "deep_success";

    std::string deepFile = helper.getTempPath("deep_nested.json");
    bool deepWriteSuccess = utils::writeJsonToFile(deepFile, deeplyNested);
    assert_true(deepWriteSuccess, "Should write deeply nested JSON");

    nlohmann::json deepReadResult;
    bool deepReadSuccess = utils::readJsonFromFile(deepFile, deepReadResult);
    assert_true(deepReadSuccess, "Should read deeply nested JSON");

    // Navigate to the deep value
    nlohmann::json *reader = &deepReadResult;
    for (int i = 0; i < 100; ++i)
    {
        reader = &((*reader)["level_" + std::to_string(i)]);
    }
    assert_true((*reader)["final_value"] == "deep_success", "Deep nested value should be preserved");

    // Test very large array
    nlohmann::json largeArray = nlohmann::json::array();
    for (int i = 0; i < 10000; ++i)
    {
        largeArray.push_back(i);
    }

    std::string arrayFile = helper.getTempPath("large_array.json");
    bool arrayWriteSuccess = utils::writeJsonToFile(arrayFile, largeArray);
    assert_true(arrayWriteSuccess, "Should write large array");

    nlohmann::json arrayReadResult;
    bool arrayReadSuccess = utils::readJsonFromFile(arrayFile, arrayReadResult);
    assert_true(arrayReadSuccess, "Should read large array");
    assert_true(arrayReadResult.size() == 10000, "Large array should maintain size");
    assert_true(arrayReadResult[5000] == 5000, "Array elements should be correct");
}

void test_concurrentFileAccess()
{
    std::cout << "\n=== Testing file access patterns ===" << std::endl;

    TempFileHelper helper;

    nlohmann::json testJson = {
        {"concurrent_test", true},
        {"timestamp", "2024-01-01T00:00:00Z"},
        {"data", nlohmann::json::array({1, 2, 3, 4, 5})}};

    std::string filename = helper.getTempPath("concurrent_test.json");

    // Write file
    bool writeSuccess = utils::writeJsonToFile(filename, testJson);
    assert_true(writeSuccess, "Initial write should succeed");

    // Multiple rapid reads
    for (int i = 0; i < 10; ++i)
    {
        nlohmann::json readResult;
        bool readSuccess = utils::readJsonFromFile(filename, readResult);
        assert_true(readSuccess, "Rapid read " + std::to_string(i) + " should succeed");
        assert_true(readResult == testJson, "Rapid read should return correct data");
    }

    // Overwrite file multiple times
    for (int i = 0; i < 5; ++i)
    {
        nlohmann::json modifiedJson = testJson;
        modifiedJson["iteration"] = i;

        bool overwriteSuccess = utils::writeJsonToFile(filename, modifiedJson);
        assert_true(overwriteSuccess, "Overwrite " + std::to_string(i) + " should succeed");

        nlohmann::json verifyRead;
        bool verifySuccess = utils::readJsonFromFile(filename, verifyRead);
        assert_true(verifySuccess, "Verification read should succeed");
        assert_true(verifyRead["iteration"] == i, "Overwritten data should be correct");
    }
}

void test_errorHandlingEdgeCases()
{
    std::cout << "\n=== Testing error handling edge cases ===" << std::endl;

    TempFileHelper helper;

    // Test malformed JSON variations
    std::vector<std::string> malformedJsons = {
        "{",                    // Incomplete object
        "}",                    // Invalid start
        "[1,2,3",               // Incomplete array
        R"({"key": "value",})", // Trailing comma
        R"({"key": value})",    // Unquoted value
        R"({'key': 'value'})",  // Single quotes
        R"({"key": "val\ue"})", // Invalid escape
        "[1,2,} 3]",            // Invalid array structure
        "undefined",            // Invalid literal
        R"({"key": 1.2.3})",    // Invalid number
    };

    for (size_t i = 0; i < malformedJsons.size(); ++i)
    {
        std::string filename = helper.createTempFile(malformedJsons[i]);

        nlohmann::json result;
        bool success = utils::readJsonFromFile(filename, result);
        assert_false(success, "Malformed JSON " + std::to_string(i) + " should fail to parse");
    }

    // Test loadJsonOrExit with malformed JSON
    std::string malformedFile = helper.createTempFile("{invalid}");

    try
    {
        nlohmann::json result = utils::loadJsonOrExit(malformedFile);
        assert_true(false, "loadJsonOrExit should throw with malformed JSON");
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "✓ loadJsonOrExit correctly threw exception for malformed JSON" << std::endl;
    }

    // Test write to read-only location (if possible)
    std::string readOnlyPath = "/proc/version"; // This should fail on most systems
    nlohmann::json testJson = {{"test", "value"}};
    bool readOnlySuccess = utils::writeJsonToFile(readOnlyPath, testJson);
    assert_false(readOnlySuccess, "Write to read-only location should fail");
}

void test_memoryAndPerformance()
{
    std::cout << "\n=== Testing memory and performance characteristics ===" << std::endl;

    TempFileHelper helper;

    // Test memory efficiency with repeated operations
    std::vector<std::chrono::milliseconds> writeTimes;
    std::vector<std::chrono::milliseconds> readTimes;

    nlohmann::json baseJson = {
        {"performance_test", true},
        {"data", nlohmann::json::array()}};

    // Add incrementally larger data sets
    for (int iteration = 0; iteration < 5; ++iteration)
    {
        // Add more data each iteration
        for (int i = 0; i < 100; ++i)
        {
            baseJson["data"].push_back({{"id", iteration * 100 + i},
                                        {"value", std::to_string(iteration * 100 + i)},
                                        {"timestamp", "2024-01-01T00:00:00Z"}});
        }

        std::string filename = helper.getTempPath("perf_test_" + std::to_string(iteration) + ".json");

        // Measure write time
        auto writeStart = std::chrono::high_resolution_clock::now();
        bool writeSuccess = utils::writeJsonToFile(filename, baseJson);
        auto writeEnd = std::chrono::high_resolution_clock::now();

        assert_true(writeSuccess, "Performance test write should succeed");

        auto writeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(writeEnd - writeStart);
        writeTimes.push_back(writeDuration);

        // Measure read time
        nlohmann::json readResult;
        auto readStart = std::chrono::high_resolution_clock::now();
        bool readSuccess = utils::readJsonFromFile(filename, readResult);
        auto readEnd = std::chrono::high_resolution_clock::now();

        assert_true(readSuccess, "Performance test read should succeed");
        assert_true(readResult == baseJson, "Performance test data should match");

        auto readDuration = std::chrono::duration_cast<std::chrono::milliseconds>(readEnd - readStart);
        readTimes.push_back(readDuration);

        std::cout << "✓ Iteration " << iteration << ": "
                  << baseJson["data"].size() << " items, "
                  << "Write: " << writeDuration.count() << "ms, "
                  << "Read: " << readDuration.count() << "ms" << std::endl;
    }

    // Verify performance doesn't degrade dramatically
    // Allow for some variation in timing, but ensure it's reasonable
    // Set minimum baseline to avoid division by zero or unrealistic comparisons
    long maxWriteTime = std::max(50L, writeTimes.front().count() * 20);
    long maxReadTime = std::max(50L, readTimes.front().count() * 20);

    assert_true(writeTimes.back().count() <= maxWriteTime,
                "Write performance should not degrade dramatically");
    assert_true(readTimes.back().count() <= maxReadTime,
                "Read performance should not degrade dramatically");
}

int main()
{
    std::cout << "🔬 Starting Utils Module Edge Case and Performance Tests" << std::endl;
    std::cout << "=========================================================" << std::endl;

    try
    {
        test_largeJsonFile();
        test_unicodeAndSpecialCharacters();
        test_edgeCaseJsonStructures();
        test_concurrentFileAccess();
        test_errorHandlingEdgeCases();
        test_memoryAndPerformance();

        std::cout << "\n🎉 All Utils edge case and performance tests passed!" << std::endl;
        std::cout << "=========================================================" << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n❌ Edge case test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\n❌ Edge case test failed with unknown exception" << std::endl;
        return 1;
    }
}
