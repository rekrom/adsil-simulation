cmake_minimum_required(VERSION 3.15)
project(adsil_analyzer)

enable_testing()  

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

