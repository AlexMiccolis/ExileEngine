add_library(ExileECS STATIC)
set(INCLUDE_SUBDIR ${PROJECT_SOURCE_DIR}/Include/Exile/ECS)
target_include_directories(ExileECS PUBLIC ${PROJECT_SOURCE_DIR}/Include)
target_sources(ExileECS PUBLIC
        ${INCLUDE_SUBDIR}/Component.hpp
        )
target_sources(ExileECS PRIVATE
        ${INCLUDE_SUBDIR}/Component.hpp
        Entity.cpp
        Component.cpp
        System.cpp
        EntityManager.cpp
        )