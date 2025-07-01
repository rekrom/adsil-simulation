file(GLOB_RECURSE UTILS_SOURCES "src/*.cpp")
file(GLOB_RECURSE UTILS_HEADERS "include/**/*.hpp")

add_library(Utils STATIC ${UTILS_SOURCES} ${UTILS_HEADERS})

target_include_directories(Utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include

)

target_link_libraries(Utils
    PUBLIC Core  # Utils modülü Core'a bağımlıysa
    Spatial
)
