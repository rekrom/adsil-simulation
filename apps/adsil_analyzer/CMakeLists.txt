file(GLOB_RECURSE APP_SOURCES "*.cpp")
file(GLOB_RECURSE APP_HEADERS "*.hpp")



add_executable(${PROJECT_NAME} ${APP_SOURCES} ${APP_HEADERS})

# Set output directory for main application
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


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
        Spatial
        Simulation
        Viewer
        Adapter
        Utils
        Math
        Vehicle
)

# Install the main executable when packaging
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# add_custom_command(
#     TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#             ${CMAKE_SOURCE_DIR}/resources
#             $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
#     COMMENT "Copying resources folder to binary output directory"
# )

