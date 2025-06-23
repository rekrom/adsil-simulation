file(GLOB_RECURSE DEVICE_SOURCES "src/*.cpp")
file(GLOB_RECURSE DEVICE_HEADERS "include/**/*.hpp")

add_library(Device STATIC ${DEVICE_SOURCES} ${DEVICE_HEADERS})

target_include_directories(Device PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Device
    PUBLIC Core  # Device modülü Core'a bağımlıysa
)
