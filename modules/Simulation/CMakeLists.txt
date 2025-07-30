file(GLOB_RECURSE SIMULATION_SOURCES "src/*.cpp")
file(GLOB_RECURSE SIMULATION_HEADERS "include/**/*.hpp")

add_library(Simulation STATIC ${SIMULATION_SOURCES} ${SIMULATION_HEADERS})
# enable_compiler_warnings(Simulation)

target_include_directories(Simulation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Simulation
    PUBLIC Core  # Simulation modülü Core'a bağımlıysa
)

target_link_libraries(Simulation
    PUBLIC Geometry  # Geometry modülü Core'a bağımlıysa
    Spatial
    Viewer
    Adapter
    Utils
)

# target_compile_definitions(Simulation PRIVATE BASE_RESOURCE_DIR=\"\$<TARGET_FILE_DIR:adsil_analyzer>/resources\")

# Add tests subdirectory
add_subdirectory(tests)