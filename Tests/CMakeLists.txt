add_executable(TLTest TL/Test.cpp TL/Benchmark.cpp)
target_include_directories(TLTest PUBLIC ${PROJECT_SOURCE_DIR}/Include)

add_test(NAME "NumericMap::Find"           COMMAND TLTest NumericMap_Find)
add_test(NAME "Template Library Benchmark" COMMAND TLTest Benchmark)

add_executable(ReflectTest Reflect/Test.cpp Reflect/Benchmark.cpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "StaticInitialize"     COMMAND ReflectTest StaticInitialize)
add_test(NAME "Field Get"            COMMAND ReflectTest FieldGet)
add_test(NAME "Field Set"            COMMAND ReflectTest FieldSet)
add_test(NAME "Reflection Benchmark" COMMAND ReflectTest Benchmark)

add_executable(ECSTest ECS/Test.cpp ECS/Benchmark.cpp)
target_link_libraries(ECSTest ExileReflect ExileECS)

add_test(NAME "Component Construction"   COMMAND ECSTest ComponentConstruction)
add_test(NAME "Entity Construction"      COMMAND ECSTest EntityConstruction)
add_test(NAME "Entity Component Search"  COMMAND ECSTest EntityComponentSearch)
add_test(NAME "ECS Benchmark"            COMMAND ECSTest Benchmark)
