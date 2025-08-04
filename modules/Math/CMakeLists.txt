file(GLOB_RECURSE MATH_SOURCES "src/*.cpp")
file(GLOB_RECURSE MATH_HEADERS "include/**/*.hpp")

add_library(Math STATIC ${MATH_SOURCES} ${MATH_HEADERS})
enable_compiler_warnings(Math)

target_include_directories(Math PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Math
    PUBLIC Geometry # Device modülü Math'a bağımlıysa
        Spatial
)

# Add tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
