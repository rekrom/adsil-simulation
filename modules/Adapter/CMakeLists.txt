file(GLOB_RECURSE ADAPTER_SOURCES "src/*.cpp")
file(GLOB_RECURSE ADAPTER_HEADERS "include/**/*.hpp")

add_library(Adapter STATIC ${ADAPTER_SOURCES} ${ADAPTER_HEADERS})

enable_compiler_warnings(Adapter)

add_subdirectory(tests)

target_include_directories(Adapter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
)

target_link_libraries(Adapter
    PUBLIC Vehicle
    PRIVATE Core
            Spatial
            Geometry
            Simulation
            Utils
)