add_library(ExileReflect STATIC)
set(INCLUDE_SUBDIR ${PROJECT_SOURCE_DIR}/Include/Exile/Reflect)
target_link_libraries(ExileReflect luajit-static)
target_include_directories(ExileReflect PUBLIC ${PROJECT_SOURCE_DIR}/Include)
target_sources(ExileReflect PUBLIC
        ${INCLUDE_SUBDIR}/Compiler.hpp
        ${INCLUDE_SUBDIR}/Reflection.hpp
        )
target_sources(ExileReflect PRIVATE
        ${INCLUDE_SUBDIR}/Impl/Invoke.hpp
        ${INCLUDE_SUBDIR}/Impl/ClassRegistry.hpp
        ClassRegistry.cpp
        Class.cpp
        )

if ("${TARGET_ARCHITECTURE}" MATCHES "x86_64")
    target_sources(ExileReflect PRIVATE Invoke.x86_64.S)
elseif ("${TARGET_ARCHITECTURE}" MATCHES "i686")
    target_sources(ExileReflect PRIVATE Invoke.i686.S)
endif ()
