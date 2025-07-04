# Gather sources and headers
file(GLOB_RECURSE VIEWER_SOURCES "src/*.cpp")
file(GLOB_RECURSE VIEWER_HEADERS "include/**/*.hpp")

# Path to ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

# ImGui core + backend source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(TINYOBJLOADER_SRC ${CMAKE_SOURCE_DIR}/external/tinyobjloader/tiny_obj_loader.cc)


# Create static library
add_library(Viewer STATIC ${VIEWER_SOURCES} ${VIEWER_HEADERS} ${IMGUI_SOURCES} ${TINYOBJLOADER_SRC})

# Public headers for consumers of Viewer
target_include_directories(Viewer
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${IMGUI_DIR}
            ${IMGUI_DIR}/backends
            ${CMAKE_SOURCE_DIR}/external/tinyobjloader  
)


# Link internal dependencies and external libraries
target_link_libraries(Viewer
    PUBLIC
        Core              # Depends on Core module
        Spatial
        Geometry
        glad              # GLAD must be linked here
        glfw              # External: GLFW
        OpenGL::GL        # External: OpenGL
        glm::glm          # External: GLM
)
