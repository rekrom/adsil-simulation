# Test files for Simulation module
file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    # Generate test name from file name
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(TEST_TARGET "test_${TEST_NAME}")

    add_executable(${TEST_TARGET} ${TEST_FILE})

    target_include_directories(${TEST_TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/modules/Simulation/include
        ${CMAKE_SOURCE_DIR}/modules/Core/include
        ${CMAKE_SOURCE_DIR}/modules/Math/include
        ${CMAKE_SOURCE_DIR}/modules/Geometry/include
        ${CMAKE_SOURCE_DIR}/modules/Vehicle/include
        ${CMAKE_SOURCE_DIR}/modules/Viewer/include
        ${CMAKE_SOURCE_DIR}/modules/Adapter/include
        ${CMAKE_SOURCE_DIR}/modules/Utils/include
        ${CMAKE_SOURCE_DIR}/modules/Spatial/include
        ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
    )

    target_link_libraries(${TEST_TARGET}
        PRIVATE Simulation Core Math Geometry Vehicle Viewer Adapter Utils Spatial
    )

    # Add test to CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_TARGET})
    
    # Copy test to bin directory
    set_target_properties(${TEST_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endforeach()
