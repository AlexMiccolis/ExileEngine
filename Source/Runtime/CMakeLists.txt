add_library(ExileRuntime SHARED)
set(INCLUDE_SUBDIR ${PROJECT_SOURCE_DIR}/Include/Exile/Runtime)
target_compile_definitions(ExileRuntime PRIVATE EXI_BUILDING_RUNTIME)
target_link_libraries(ExileRuntime luajit-static)
target_include_directories(ExileRuntime PUBLIC ${PROJECT_SOURCE_DIR}/Include)
target_sources(ExileRuntime PUBLIC
        ${INCLUDE_SUBDIR}/API.hpp
        ${INCLUDE_SUBDIR}/LuaContext.hpp
        )
target_sources(ExileRuntime PRIVATE
        LuaContext.cpp
        )
set_target_properties(ExileRuntime PROPERTIES PREFIX "")
set_target_properties(ExileRuntime PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set_target_properties(ExileRuntime PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
