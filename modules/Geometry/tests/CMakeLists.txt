# Geometry Module Tests

# Create test executables for each major component
add_executable(test_CubeTest CubeTest.cpp)
add_executable(test_CylinderTest CylinderTest.cpp)
add_executable(test_DeviceTest DeviceTest.cpp)
add_executable(test_ShapeFactoryTest ShapeFactoryTest.cpp)
add_executable(test_DeviceFactoryTest DeviceFactoryTest.cpp)
add_executable(test_GeometryIntegrationTest GeometryIntegrationTest.cpp)

# Enable compiler warnings for all tests
enable_compiler_warnings(test_CubeTest)
enable_compiler_warnings(test_CylinderTest)
enable_compiler_warnings(test_DeviceTest)
enable_compiler_warnings(test_ShapeFactoryTest)
enable_compiler_warnings(test_DeviceFactoryTest)
enable_compiler_warnings(test_GeometryIntegrationTest)

# Link all tests with required libraries
set(GEOMETRY_TEST_LIBS Geometry Math Spatial Core)

target_link_libraries(test_CubeTest PRIVATE ${GEOMETRY_TEST_LIBS})
target_link_libraries(test_CylinderTest PRIVATE ${GEOMETRY_TEST_LIBS})
target_link_libraries(test_DeviceTest PRIVATE ${GEOMETRY_TEST_LIBS})
target_link_libraries(test_ShapeFactoryTest PRIVATE ${GEOMETRY_TEST_LIBS})
target_link_libraries(test_DeviceFactoryTest PRIVATE ${GEOMETRY_TEST_LIBS})
target_link_libraries(test_GeometryIntegrationTest PRIVATE ${GEOMETRY_TEST_LIBS})

# Include directories for all tests
foreach(test_target test_CubeTest test_CylinderTest test_DeviceTest test_ShapeFactoryTest test_DeviceFactoryTest test_GeometryIntegrationTest)
    target_include_directories(${test_target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_SOURCE_DIR}/modules/Math/include
        ${CMAKE_SOURCE_DIR}/modules/Spatial/include
        ${CMAKE_SOURCE_DIR}/modules/Core/include
    )
endforeach()

# Add tests to CTest
add_test(NAME CubeTest COMMAND test_CubeTest)
add_test(NAME CylinderTest COMMAND test_CylinderTest)
add_test(NAME DeviceTest COMMAND test_DeviceTest)
add_test(NAME ShapeFactoryTest COMMAND test_ShapeFactoryTest)
add_test(NAME DeviceFactoryTest COMMAND test_DeviceFactoryTest)
add_test(NAME GeometryIntegrationTest COMMAND test_GeometryIntegrationTest)

# Set test properties
foreach(test_name CubeTest CylinderTest DeviceTest ShapeFactoryTest DeviceFactoryTest GeometryIntegrationTest)
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 30
        LABELS "geometry;shapes;3d"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endforeach()

# Add specific labels for specialized tests
set_tests_properties(DeviceTest PROPERTIES LABELS "geometry;devices;fov;lidar")
set_tests_properties(ShapeFactoryTest PROPERTIES LABELS "geometry;factory;patterns")
set_tests_properties(DeviceFactoryTest PROPERTIES LABELS "geometry;factory;devices;patterns")
set_tests_properties(GeometryIntegrationTest PROPERTIES 
    LABELS "geometry;integration;end-to-end"
    TIMEOUT 120
)
