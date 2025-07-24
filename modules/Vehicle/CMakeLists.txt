file(GLOB_RECURSE VEHICLE_SOURCES "src/*.cpp")
file(GLOB_RECURSE VEHICLE_HEADERS "include/**/*.hpp")

add_library(Vehicle STATIC ${VEHICLE_SOURCES} ${VEHICLE_HEADERS})
enable_compiler_warnings(Vehicle)

target_include_directories(Vehicle PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Vehicle
    PUBLIC Geometry # Device modülü Vehicle'a bağımlıysa
        Core
        Spatial
        Math
)
