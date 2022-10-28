cmake_minimum_required(VERSION 3.23)
project(ExileEngine CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

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
            add_compile_options(-march=nehalem)
        elseif()
            message(FATAL_ERROR "Unsupported architecture: ${ARCHITECTURE}")
        endif ()
    elseif()
        # TODO: Support for other compilers
        message(FATAL_ERROR "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

get_architecture()
add_subdirectory(Dependencies)
add_subdirectory(Source)

enable_testing()
add_subdirectory(Tests)
