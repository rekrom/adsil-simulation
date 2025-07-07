# cmake/CompilerWarnings.cmake
function(enable_compiler_warnings target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -Wconversion)
    elseif (MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()
