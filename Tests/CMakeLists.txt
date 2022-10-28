add_executable(TLTest TL/Test.cpp TL/Benchmark.cpp)
target_include_directories(TLTest PUBLIC ${PROJECT_SOURCE_DIR}/Include)

add_test(NAME "[TL] NumericMap::Find"           COMMAND TLTest NumericMap_Find)
add_test(NAME "[TL] NumericMap::GetKeys"        COMMAND TLTest NumericMap_GetKeys)
add_test(NAME "[TL] NumericMap::Contract"       COMMAND TLTest NumericMap_Contract)
add_test(NAME "[TL] FreeMap::Allocate"          COMMAND TLTest FreeMap_Allocate)
add_test(NAME "[TL] Template Library Benchmark" COMMAND TLTest Benchmark)

add_executable(ReflectTest Reflect/Test.cpp Reflect/Benchmark.cpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "[Reflect] StaticInitialize"     COMMAND ReflectTest StaticInitialize)
add_test(NAME "[Reflect] Field Get"            COMMAND ReflectTest FieldGet)
add_test(NAME "[Reflect] Field Set"            COMMAND ReflectTest FieldSet)
add_test(NAME "[Reflect] Reflection Benchmark" COMMAND ReflectTest Benchmark)

add_executable(ECSTest ECS/Test.cpp ECS/Benchmark.cpp)
target_link_libraries(ECSTest ExileReflect ExileECS)

add_test(NAME "[ECS] Component Construction"   COMMAND ECSTest ComponentConstruction)
add_test(NAME "[ECS] Entity Construction"      COMMAND ECSTest EntityConstruction)
add_test(NAME "[ECS] Entity Component Search"  COMMAND ECSTest EntityComponentSearch)
add_test(NAME "[ECS] ECS Benchmark"            COMMAND ECSTest Benchmark)
