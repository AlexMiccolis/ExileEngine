#include <Exile/Unit/Benchmark.hpp>
#include <Exile/Runtime/Filesystem.hpp>

Exi::Unit::BenchmarkResults Benchmark_Filesystem_TranslatePath()
{
    Exi::Runtime::Filesystem fs(std::filesystem::current_path().string());
    Exi::Runtime::Path path;

    std::filesystem::create_directories("Test/1/2/3");
    fs.MountDirectory("Test/1",     "/Test1");
    fs.MountDirectory("Test/1/2",   "/Test1/Test2");
    fs.MountDirectory("Test/1/2/3", "/Test1/Test2/Test3");

    BENCHMARK_START(Filesystem_TranslatePath, 65536);
    BENCHMARK_LOOP(Filesystem_TranslatePath)
    {
        if (!fs.TranslatePath("Test1/Test2/Test.txt", path))
        {
            BENCHMARK_FAIL(Filesystem_TranslatePath);
            break;
        }
    }
    return BENCHMARK_END(Filesystem_TranslatePath);
}

bool Benchmark()
{
    Exi::Unit::RunBenchmark("Filesystem::TranslatePath", Benchmark_Filesystem_TranslatePath);

    return true;
}
