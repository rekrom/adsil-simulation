# Test dosyalarını tara
file(GLOB TEST_SOURCES "*.cpp")

foreach(TEST_FILE ${TEST_SOURCES})
    # Dosya adından test adı üret
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    set(TEST_TARGET "test_${TEST_NAME}")

    add_executable(${TEST_TARGET} ${TEST_FILE})

    target_include_directories(${TEST_TARGET} PRIVATE
        ${CMAKE_SOURCE_DIR}/modules/Adapter/include
        ${CMAKE_SOURCE_DIR}/modules/Core/include
        ${CMAKE_SOURCE_DIR}/external/nlohmann_json/single_include
    )

    target_link_libraries(${TEST_TARGET}
        PRIVATE Adapter Core
    )

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

endforeach()
