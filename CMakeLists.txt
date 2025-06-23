cmake_minimum_required(VERSION 3.15)
project(adsil_analyzer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include dir for legacy includes (if needed by main.cpp)
include_directories(include)

# GLAD integration
set(GLAD_SOURCES external/glad/src/glad.c)
include_directories(external/glad/include)

# Source files for root-level main.cpp (if any)
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# External dependencies: MUST be found before Viewer
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Add module subdirectories
add_subdirectory(apps/adsil_analyzer)
add_subdirectory(modules/Core)
add_subdirectory(modules/Geometry)
add_subdirectory(modules/Device)
add_subdirectory(modules/Simulation)
# add_subdirectory(modules/Solver)
add_subdirectory(modules/Viewer)
