#include <Exile/Unit/Benchmark.hpp>
#include <Exile/Runtime/Filesystem.hpp>
#include <filesystem>

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

Exi::Unit::BenchmarkResults Benchmark_Path_Constructor()
{
    BENCHMARK_START(Path_Constructor, 65536);
    BENCHMARK_LOOP(Path_Constructor)
    {
        Exi::Runtime::Path path("dir/1/2/3/test.bin");
    }
    return BENCHMARK_END(Path_Constructor);
}

bool Benchmark()
{
    Exi::Unit::RunBenchmark("Filesystem::TranslatePath", Benchmark_Filesystem_TranslatePath);
    Exi::Unit::RunBenchmark("Path::Path", Benchmark_Path_Constructor);

    return true;
}
