# Gather sources and headers
file(GLOB_RECURSE VIEWER_SOURCES "src/*.cpp")
file(GLOB_RECURSE VIEWER_HEADERS "include/**/*.hpp")

# Create static library
add_library(Viewer STATIC ${VIEWER_SOURCES} ${VIEWER_HEADERS})

# Include path for other modules to consume Viewer headers
target_include_directories(Viewer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link internal dependencies and external libraries
target_link_libraries(Viewer
    PUBLIC
        Core              # Depends on Core module
        glfw              # External: GLFW
        OpenGL::GL        # External: OpenGL
        glm::glm          # External: GLM
)
