# Gather sources and headers
file(GLOB_RECURSE SPATIAL_SOURCES "src/*.cpp")
file(GLOB_RECURSE SPATIAL_HEADERS "include/**/*.hpp")

# Create static library
add_library(Spatial STATIC ${SPATIAL_SOURCES} ${SPATIAL_HEADERS})
enable_compiler_warnings(Spatial)

# Public headers for consumers of Spatial
target_include_directories(Spatial
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link internal dependencies and external libraries
target_link_libraries(Spatial PUBLIC
        Math
)
