#pragma once

#include <chrono>
#include <cstdio>

namespace Exi::Unit
{

    struct BenchmarkResults
    {
        std::size_t IterationsCount;
        std::size_t NanosPerIteration;
        double TotalSeconds;
        bool Failed;

        BenchmarkResults(std::size_t Iterations, double Seconds, bool Fail = false)
            : IterationsCount(Iterations), TotalSeconds(Seconds),
            NanosPerIteration(static_cast<std::size_t>(Seconds / Iterations / 1e-9)), Failed(Fail) { }
    };

    static inline bool RunBenchmark(const char* Name, Exi::Unit::BenchmarkResults(*Fn)())
    {
        auto Results = Fn();
        if (Results.Failed)
        {
            printf("%s: FAILED\n", Name);
            return false;
        }

        printf("%s: %lu iterations, %lu ns/iteration, %.07f total seconds\n",
               Name,
               Results.IterationsCount,
               Results.NanosPerIteration,
               Results.TotalSeconds);
        return true;
    }

};

#define BENCHMARK_START(Name, Iterations) auto Benchmark_##Name##_Start = std::chrono::high_resolution_clock::now(); \
    constexpr std::size_t Benchmark_##Name##_Iterations = Iterations; \
    bool Benchmark_##Name##_Failure = false;

#define BENCHMARK_LOOP(Name) for (std::size_t Iteration = 0; Iteration < Benchmark_##Name##_Iterations; Iteration++)
#define BENCHMARK_FAIL(Name) Benchmark_##Name##_Failure = true;

#define BENCHMARK_END(Name) \
    Exi::Unit::BenchmarkResults( \
    Benchmark_##Name##_Iterations, \
    std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - Benchmark_##Name##_Start).count(), \
    Benchmark_##Name##_Failure)


