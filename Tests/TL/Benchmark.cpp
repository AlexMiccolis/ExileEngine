#include <cstdio>
#include <Exile/Unit/Benchmark.hpp>
#include <Exile/TL/NumericMap.hpp>

Exi::Unit::BenchmarkResults Benchmark_NumericMap_Find()
{
    constexpr std::size_t count = 8;
    Exi::TL::NumericMap<std::size_t, int> map;
    int values[count] = { 0 };

    for (auto i = 0; i < count; i++)
    {
        map.Emplace(0, i);
        map.Emplace(1, i);
        map.Emplace(2, i);
        map.Emplace(3, i);
    }

    BENCHMARK_START(NumericMap_Find, 65536 * 16);
    BENCHMARK_LOOP(NumericMap_Find)
    {
        if (map.Find(2, values, count) != count)
        {
            BENCHMARK_FAIL(NumericMap_Find);
            break;
        }
    }
    return BENCHMARK_END(NumericMap_Find);
}

bool RunBenchmark(const char* Name, Exi::Unit::BenchmarkResults(*Fn)())
{
    auto Results = Fn();
    if (Results.Failed)
    {
        printf("%s: FAILED\n", Name);
        return false;
    }

    printf("%s: %llu iterations, %llu ns/iteration, %.07f total seconds\n",
           Name,
           Results.IterationsCount,
           Results.NanosPerIteration,
           Results.TotalSeconds);
    return true;
}

bool Benchmark()
{
    RunBenchmark("NumericMap::Find", Benchmark_NumericMap_Find);

    return true;
}
