add_executable(TLTest TL/Test.cpp TL/Benchmark.cpp)
target_include_directories(TLTest PUBLIC ${PROJECT_SOURCE_DIR}/Include)

add_test(NAME "[TL] _ Template Library Benchmark _" COMMAND TLTest Benchmark)
add_test(NAME "[TL] NumericMap::Find"               COMMAND TLTest NumericMap_Find)
add_test(NAME "[TL] NumericMap::GetKeys"            COMMAND TLTest NumericMap_GetKeys)
add_test(NAME "[TL] NumericMap::Contract"           COMMAND TLTest NumericMap_Contract)
add_test(NAME "[TL] FreeMap::Allocate"              COMMAND TLTest FreeMap_Allocate)
add_test(NAME "[TL] ByteUtils::PopCount"            COMMAND TLTest ByteUtils_PopCount)
add_test(NAME "[TL] ByteUtils::FindFirstSet"        COMMAND TLTest ByteUtils_FindFirstSet)
add_test(NAME "[TL] UUID::Random"                   COMMAND TLTest UUID_Random)

add_executable(RuntimeTest Runtime/Test.cpp)
target_link_libraries(RuntimeTest ExileRuntime)

add_test(NAME "[Runtime] LuaContext::ExecuteString"     COMMAND RuntimeTest LuaContext_ExecuteString)
add_test(NAME "[Runtime] LuaContext::SetGlobalFunction" COMMAND RuntimeTest LuaContext_SetGlobalFunction)
add_test(NAME "[Runtime] PathUtils::GetFirstFragment"   COMMAND RuntimeTest PathUtils_GetFirstFragment)
add_test(NAME "[Runtime] PathUtils::StripSeparators"    COMMAND RuntimeTest PathUtils_StripSeparators)

add_executable(ReflectTest Reflect/Test.cpp Reflect/Benchmark.cpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "[Reflect] _ Reflection Benchmark _"  COMMAND ReflectTest Benchmark)
add_test(NAME "[Reflect] StaticInitialize"          COMMAND ReflectTest StaticInitialize)
add_test(NAME "[Reflect] Field Get"                 COMMAND ReflectTest FieldGet)
add_test(NAME "[Reflect] Field Set"                 COMMAND ReflectTest FieldSet)

add_executable(ECSTest ECS/Test.cpp ECS/Benchmark.cpp)
target_link_libraries(ECSTest ExileReflect ExileECS)

add_test(NAME "[ECS] _ ECS Benchmark _"           COMMAND ECSTest Benchmark)
add_test(NAME "[ECS] Component Construction"      COMMAND ECSTest ComponentConstruction)
add_test(NAME "[ECS] Entity Construction"         COMMAND ECSTest EntityConstruction)
add_test(NAME "[ECS] Entity Component Search"     COMMAND ECSTest EntityComponentSearch)
add_test(NAME "[ECS] EntityManager::GetEntity"    COMMAND ECSTest EntityManagerGetEntity)

set_target_properties(TLTest RuntimeTest ReflectTest ECSTest
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
