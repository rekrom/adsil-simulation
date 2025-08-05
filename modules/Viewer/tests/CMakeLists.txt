# Test files for Viewer module
file(GLOB TEST_SOURCES "*.cpp")

# Get module name from current directory
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(MODULE_NAME ${PARENT_DIR} NAME)

# Create the output directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests/${MODULE_NAME})

foreach(TEST_FILE ${TEST_SOURCES})
    # Generate test name from file name
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(TEST_TARGET "test_${TEST_NAME}")

    add_executable(${TEST_TARGET} ${TEST_FILE})

    target_include_directories(${TEST_TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/modules/Viewer/include
        ${CMAKE_SOURCE_DIR}/modules/Core/include
        ${CMAKE_SOURCE_DIR}/modules/Vehicle/include
        ${CMAKE_SOURCE_DIR}/modules/Math/include
        ${CMAKE_SOURCE_DIR}/modules/Spatial/include
        ${CMAKE_SOURCE_DIR}/modules/Geometry/include
        ${CMAKE_SOURCE_DIR}/modules/Simulation/include
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/external/tinyobjloader
    )

    target_link_libraries(${TEST_TARGET}
        PRIVATE Viewer Core Vehicle Math Spatial Geometry Simulation
                glad glfw OpenGL::GL glm::glm
    )

    # Set output directory for test executables
    set_target_properties(${TEST_TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests/${MODULE_NAME}
    )

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

endforeach()
