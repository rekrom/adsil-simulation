file(GLOB_RECURSE GEOMETRY_SOURCES "src/*.cpp")
file(GLOB_RECURSE GEOMETRY_HEADERS "include/**/*.hpp")

add_library(Geometry STATIC ${GEOMETRY_SOURCES} ${GEOMETRY_HEADERS})
enable_compiler_warnings(Geometry)

target_include_directories(Geometry PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Geometry
    PUBLIC Core  # Geometry modülü Core'a bağımlıysa
        Spatial
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()