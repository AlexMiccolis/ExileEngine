#include <cstdio>
#include <Exile/Unit/Benchmark.hpp>
#include <Exile/TL/NumericMap.hpp>
#include <Exile/TL/FreeMap.hpp>

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

Exi::Unit::BenchmarkResults Benchmark_NumericMap_GetKeys()
{
    constexpr std::size_t count = 8;
    Exi::TL::NumericMap<std::size_t, int> map;

    for (auto i = 0; i < count; i++)
    {
        map.Emplace(0, i);
        map.Emplace(1, i);
        map.Emplace(2, i);
        map.Emplace(3, i);
    }

    BENCHMARK_START(NumericMap_GetKeys, 65536 * 16);
    BENCHMARK_LOOP(NumericMap_GetKeys)
    {
        if (map.GetKeys() != 4)
        {
            BENCHMARK_FAIL(NumericMap_GetKeys);
            break;
        }
    }
    return BENCHMARK_END(NumericMap_GetKeys);
}

Exi::Unit::BenchmarkResults Benchmark_FreeMap_Allocate()
{
    constexpr std::size_t count = 8;
    Exi::TL::FreeMap<64> map;

    BENCHMARK_START(FreeMap_Allocate, 65536 * 16);
    BENCHMARK_LOOP(FreeMap_Allocate)
    {
        auto i = map.Allocate();
        if (i != 0)
        {
            BENCHMARK_FAIL(FreeMap_Allocate);
            break;
        }
        map.Free(i);
    }
    return BENCHMARK_END(FreeMap_Allocate);
}

bool Benchmark()
{
    Exi::Unit::RunBenchmark("NumericMap::Find",    Benchmark_NumericMap_Find);
    Exi::Unit::RunBenchmark("NumericMap::GetKeys", Benchmark_NumericMap_GetKeys);
    Exi::Unit::RunBenchmark("FreeMap::Allocate",   Benchmark_FreeMap_Allocate);

    return true;
}
