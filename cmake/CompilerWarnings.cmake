# cmake/CompilerWarnings.cmake
function(enable_compiler_warnings target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE 
            -Wall
            -Wextra
            -Wpedantic
            -Werror
        
            # Safety-related
            -Wshadow
            -Wconversion
            # -Wsign-conversion
            # -Wnull-dereference
            # -Wnon-virtual-dtor
            # -Wdouble-promotion
            # -Wformat=2

             # Logic-related
            # -Wlogical-op
            # -Wduplicated-cond
            # -Wmisleading-indentation
            # -Wimplicit-fallthrough
            # -Wuseless-cast

            # Optional (remove if noisy)
            # -Wno-unused-parameter
            )
                    # Compiler-version specific flags
            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13)
                target_compile_options(${target} PRIVATE -Wdangling-reference)
            endif()
    elseif (MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    endif()
endfunction()
