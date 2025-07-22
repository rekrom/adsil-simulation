file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/**/*.hpp")

add_library(Core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
enable_compiler_warnings(Core)

target_include_directories(Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Core
    PUBLIC Geometry # Device modülü Core'a bağımlıysa
        Spatial
        Math
)
