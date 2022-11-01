add_executable(ReflectTest Test.cpp Benchmark.cpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "[Reflect] _ Reflection Benchmark _"  COMMAND ReflectTest Benchmark)
add_test(NAME "[Reflect] StaticInitialize"          COMMAND ReflectTest StaticInitialize)
add_test(NAME "[Reflect] Field Get"                 COMMAND ReflectTest FieldGet)
add_test(NAME "[Reflect] Field Set"                 COMMAND ReflectTest FieldSet)
set_target_properties(ReflectTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
