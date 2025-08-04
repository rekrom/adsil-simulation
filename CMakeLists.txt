cmake_minimum_required(VERSION 3.15)
project(adsil_analyzer)

enable_testing()  

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# External dependencies
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# GLAD as static lib
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)

# Coverage flags
option(COVERAGE "Enable coverage flags" ON)
if(COVERAGE)
    message(STATUS "Building with coverage flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" CACHE STRING "" FORCE)
endif()

# Automatically add all modules
file(GLOB MODULE_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/modules" "${CMAKE_SOURCE_DIR}/modules/*")
foreach(module_dir ${MODULE_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/modules/${module_dir}")
        add_subdirectory("modules/${module_dir}")
    endif()
endforeach()

# Automatically add all apps
file(GLOB APP_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/apps" "${CMAKE_SOURCE_DIR}/apps/*")
foreach(app_dir ${APP_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/apps/${app_dir}")
        add_subdirectory("apps/${app_dir}")
    endif()
endforeach()

