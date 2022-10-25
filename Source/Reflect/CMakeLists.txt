add_library(ExileReflect STATIC)
set(INCLUDE_SUBDIR ${PROJECT_SOURCE_DIR}/Include/Exile/Reflect)
target_include_directories(ExileReflect PUBLIC ${PROJECT_SOURCE_DIR}/Include)
target_sources(ExileReflect PUBLIC
        ${INCLUDE_SUBDIR}/Compiler.hpp
        ${INCLUDE_SUBDIR}/Reflection.hpp
        )
target_sources(ExileReflect PRIVATE
        ${INCLUDE_SUBDIR}/Impl/Type.hpp
        ${INCLUDE_SUBDIR}/Impl/ClassRegistry.hpp
        ClassRegistry.cpp
        )
