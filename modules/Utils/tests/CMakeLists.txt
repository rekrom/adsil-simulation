# Utils Module Tests

# Create test executable
add_executable(test_UtilsTest UtilsTest.cpp)

# Enable compiler warnings for tests
enable_compiler_warnings(test_UtilsTest)

# Link the test with the Utils library and its dependencies
target_link_libraries(test_UtilsTest 
    PRIVATE 
        Utils
)

# Include directories
target_include_directories(test_UtilsTest 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
)

# Create edge case test executable
add_executable(test_UtilsEdgeCaseTest UtilsEdgeCaseTest.cpp)

# Enable compiler warnings for edge case tests
enable_compiler_warnings(test_UtilsEdgeCaseTest)

# Link the edge case test with the Utils library and its dependencies
target_link_libraries(test_UtilsEdgeCaseTest 
    PRIVATE 
        Utils
)

# Include directories for edge case tests
target_include_directories(test_UtilsEdgeCaseTest 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
)

# Add test to CTest
add_test(NAME UtilsTest COMMAND test_UtilsTest)
add_test(NAME UtilsEdgeCaseTest COMMAND test_UtilsEdgeCaseTest)

# Set test properties
set_tests_properties(UtilsTest PROPERTIES
    TIMEOUT 30
    LABELS "utils;json;filesystem"
)

set_tests_properties(UtilsEdgeCaseTest PROPERTIES
    TIMEOUT 60
    LABELS "utils;json;filesystem;performance;edge-cases"
)

# Ensure test runs in build directory for temporary file access
set_tests_properties(UtilsTest PROPERTIES 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_tests_properties(UtilsEdgeCaseTest PROPERTIES 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
