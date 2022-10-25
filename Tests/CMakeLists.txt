add_executable(ReflectTest Reflect/Test.cpp Reflect/Benchmark.cpp Reflect/Benchmark.hpp)
target_link_libraries(ReflectTest ExileReflect)

add_test(NAME "StaticInitialize"   COMMAND ReflectTest StaticInitialize)
add_test(NAME "Field Get"          COMMAND ReflectTest FieldGet)
add_test(NAME "Field Set"          COMMAND ReflectTest FieldSet)
add_test(NAME "Benchmark"          COMMAND ReflectTest Benchmark)
