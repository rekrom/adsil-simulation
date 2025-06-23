file(GLOB_RECURSE JSON_SOURCES "src/*.cpp")
file(GLOB_RECURSE JSON_HEADERS "include/**/*.hpp")

add_library(JsonUtils STATIC ${JSON_SOURCES} ${JSON_HEADERS})

target_include_directories(JsonUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Header-only olduğu için yalnızca include yeterli
target_include_directories(JsonUtils PUBLIC
    ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
)
