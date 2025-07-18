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


# Create static library from Viewer-only sources
add_library(Viewer STATIC ${VIEWER_SOURCES} ${VIEWER_HEADERS})

# Add ImGui and TinyObjLoader sources separately
target_sources(Viewer PRIVATE ${IMGUI_SOURCES} ${TINYOBJLOADER_SRC})

# Disable warnings for ImGui and TinyObjLoader sources
foreach(source_file IN LISTS IMGUI_SOURCES)
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "-w")
endforeach()
set_source_files_properties(${TINYOBJLOADER_SRC} PROPERTIES COMPILE_FLAGS "-w")

# Enable warnings for Viewer sources
enable_compiler_warnings(Viewer)

# Public headers for consumers of Viewer
target_include_directories(Viewer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
        Simulation
        glad              # GLAD must be linked here
        glfw              # External: GLFW
        OpenGL::GL        # External: OpenGL
        glm::glm          # External: GLM
)