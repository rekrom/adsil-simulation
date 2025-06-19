cmake_minimum_required(VERSION 3.15)
project(adsil_analyzer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically include source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# GLAD manual integration
set(GLAD_SOURCES external/glad/src/glad.c)
include_directories(external/glad/include)

# Add include directory
include_directories(include)

# Set CMAKE_PREFIX_PATH for GLFW
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/lib/x86_64-linux-gnu/cmake/glfw3")

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Build executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLAD_SOURCES})

# # Static Library
# add_library(Core STATUC
# src/Core/Vector.cpp
# src/Core/Point.cpp
# )
# target_include_directories(Core PUBLIC
#     ${CMAKE_SOURCE_DIR}/include
# )

# Link everything
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL
        glm::glm
)
