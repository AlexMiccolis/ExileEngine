cmake_minimum_required(VERSION 3.23)
project(ExileEngine CXX ASM)

set(CMAKE_CXX_STANDARD 20)

# Run assembler files through the C++ Compiler
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof")
endif()

function (get_architecture)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        execute_process( COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine OUTPUT_VARIABLE ARCHITECTURE )
        if ("${ARCHITECTURE}" MATCHES "^(x86_64)")
            set(TARGET_ARCHITECTURE "x86_64-gnu" CACHE STRING "" FORCE)
        elseif()
            set(TARGET_ARCHITECTURE "unknown" CACHE STRING "" FORCE)
        endif ()
    elseif()
        # TODO: Support for other compilers
    endif()
endfunction()

get_architecture()
message(WARNING "Target Architecture: ${TARGET_ARCHITECTURE}")

add_subdirectory(Dependencies)
add_subdirectory(Source)

enable_testing()
add_subdirectory(Tests)
