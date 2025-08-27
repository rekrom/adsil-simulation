cmake_minimum_required(VERSION 3.15)
project(adsil_analyzer VERSION 0.1.0)

enable_testing()  

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default resource path (relative to repo root)
set(ADSIL_RESOURCE_PATH_DEFAULT "${CMAKE_SOURCE_DIR}/resources" CACHE PATH "Default path to resources")
set(ROOT_PATH_DEFAULT "${CMAKE_SOURCE_DIR}" CACHE PATH "Default path to root folder")

# Pass it into C++ as a definition
add_compile_definitions(ADSIL_RESOURCE_PATH_DEFAULT="${ADSIL_RESOURCE_PATH_DEFAULT}")
add_compile_definitions(ROOT_PATH_DEFAULT="${ROOT_PATH_DEFAULT}")

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


# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified, defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    # Optional: restrict options
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()


# ------------------------
# Distribution / Packaging
# ------------------------
option(ADSIL_DISTRIBUTION "Build optimized, stripped distribution binary" OFF)

if(ADSIL_DISTRIBUTION)
    # Force Release
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Forcing Release build for distribution")
        set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    endif()
    add_compile_options(-O3 -DNDEBUG -fvisibility=hidden)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        message(STATUS "IPO/LTO enabled for distribution build")
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else()
        message(WARNING "IPO/LTO not supported: ${ipo_error}")
    endif()
endif()

# Install resources (exclude volatile/log folders)
install(DIRECTORY resources/ DESTINATION resources
    PATTERN "logs" EXCLUDE
    PATTERN "logs/*" EXCLUDE
    PATTERN "extracted_frames_json" EXCLUDE
    PATTERN "extracted_frames_json/*" EXCLUDE)

# Wrapper run script (no substitution needed) installed at package root
install(PROGRAMS ${CMAKE_SOURCE_DIR}/cmake/run_adsil.sh.in DESTINATION . RENAME run_adsil.sh)

# CPack configuration (binary-only package)
set(CPACK_PACKAGE_NAME "adsil_analyzer")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "rekrom")
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-linux-x86_64")
set(CPACK_STRIP_FILES TRUE)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_PACKAGE_CONTACT "")
include(CPack)

