add_executable(ReflectTest Reflect/Test.cpp Reflect/Benchmark.cpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "StaticInitialize"   COMMAND ReflectTest StaticInitialize)
add_test(NAME "Field Get"          COMMAND ReflectTest FieldGet)
add_test(NAME "Field Set"          COMMAND ReflectTest FieldSet)
add_test(NAME "Benchmark"          COMMAND ReflectTest Benchmark)

add_executable(ECSTest ECS/Test.cpp)
target_link_libraries(ECSTest ExileReflect ExileECS)

add_test(NAME "ComponentConstruction"   COMMAND ECSTest ComponentConstruction)
