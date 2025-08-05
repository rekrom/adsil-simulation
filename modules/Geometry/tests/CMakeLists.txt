# Test files for Geometry module
file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    # Generate test name from file name
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(TEST_TARGET "test_${TEST_NAME}")

    add_executable(${TEST_TARGET} ${TEST_FILE})

    target_include_directories(${TEST_TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/modules/Geometry/include
        ${CMAKE_SOURCE_DIR}/modules/Math/include
        ${CMAKE_SOURCE_DIR}/modules/Spatial/include
        ${CMAKE_SOURCE_DIR}/modules/Core/include
    )

    target_link_libraries(${TEST_TARGET}
        PRIVATE Geometry Math Spatial Core
    )

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

endforeach()
