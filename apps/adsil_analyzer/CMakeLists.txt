file(GLOB_RECURSE APP_SOURCES "*.cpp")
file(GLOB_RECURSE APP_HEADERS "*.hpp")
project(adsil_analyzer)

# Set output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})

# GLAD
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/external/glad/include)

# Link to all required modules
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenGL::GL
        glm::glm
        Core
        Geometry
        Simulation
        Viewer
        Adapter
        Utils
)

add_custom_command(
    TARGET adsil_analyzer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:adsil_analyzer>/resources
    COMMENT "Copying resources folder to binary output directory"
)
