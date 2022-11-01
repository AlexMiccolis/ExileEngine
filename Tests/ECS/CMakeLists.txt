add_executable(ECSTest Test.cpp Benchmark.cpp)
target_link_libraries(ECSTest ExileReflect ExileECS)

add_test(NAME "[ECS] _ ECS Benchmark _"           COMMAND ECSTest Benchmark)
add_test(NAME "[ECS] Component Construction"      COMMAND ECSTest ComponentConstruction)
add_test(NAME "[ECS] Entity Construction"         COMMAND ECSTest EntityConstruction)
add_test(NAME "[ECS] Entity Component Search"     COMMAND ECSTest EntityComponentSearch)
add_test(NAME "[ECS] EntityManager::GetEntity"    COMMAND ECSTest EntityManagerGetEntity)
set_target_properties(ECSTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
