add_library(ExileRuntime SHARED)
set(INCLUDE_SUBDIR ${PROJECT_SOURCE_DIR}/Include/Exile/Runtime)
target_compile_definitions(ExileRuntime PRIVATE EXI_BUILDING_RUNTIME)
target_link_libraries(ExileRuntime luajit-pic-static)
target_include_directories(ExileRuntime PUBLIC ${PROJECT_SOURCE_DIR}/Include)
target_sources(ExileRuntime PUBLIC
        ${INCLUDE_SUBDIR}/API.hpp
        ${INCLUDE_SUBDIR}/Logger.hpp
        ${INCLUDE_SUBDIR}/LuaState.hpp
        ${INCLUDE_SUBDIR}/Filesystem.hpp
        )
target_sources(ExileRuntime PRIVATE
        Filesystem/Path.cpp
        Logger.cpp
        Lua/LuaState.cpp
        Filesystem/Filesystem.cpp
        Filesystem/FileHandle.cpp
        Filesystem/FileControl.cpp
        )
set_target_properties(ExileRuntime PROPERTIES PREFIX "")
set_target_properties(ExileRuntime PROPERTIES OUTPUT_NAME "ExiRT")
set_target_properties(ExileRuntime PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set_target_properties(ExileRuntime PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
