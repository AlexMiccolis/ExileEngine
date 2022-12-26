#include <Exile/Unit/Test.hpp>
#include <Exile/Runtime/Lua.hpp>
#include <Exile/Runtime/Filesystem.hpp>
#include <Exile/Runtime/Reflection.hpp>
#include <filesystem>
#include <thread>
#include <cstring>

extern bool Benchmark();

bool Test_PathUtils_GetNextFragment()
{
    const std::string path = "/\\a////b\\/c/";
    std::size_t index = 0;
    auto fragment = Exi::Runtime::PathUtils::GetNextFragment(path, index);
    std::vector<std::string_view> fragments;

    while (!fragment.empty())
    {
        fragments.push_back(fragment);
        fragment = Exi::Runtime::PathUtils::GetNextFragment(path, index);
    }

    if (fragments.size() != 3)
        return false;

    if (fragments[0] != "a" || fragments[1] != "b" || fragments[2] != "c")
        return false;

    return true;
}

bool Test_PathUtils_StripSeparators()
{
    std::string test1 = "\\\\test1";
    std::string test2 = "\\\\//test2////";
    std::string test3 = "\\\\//te/st3////";

    auto result1 = Exi::Runtime::PathUtils::StripSeparators(test1);
    auto result2 = Exi::Runtime::PathUtils::StripSeparators(test2);
    auto result3 = Exi::Runtime::PathUtils::StripSeparators(test3);

    if (result1 != "test1" || result2 != "test2" || result3 != "te")
        return false;

    return true;
}

bool Test_Path_Constructor()
{
    Exi::Runtime::Path path1("///test//path/test.txt");
    Exi::Runtime::Path path2("///test\\directory\\");
    Exi::Runtime::Path path3("///test\\directory");
    Exi::Runtime::Path path4("test/test.txt");

    if (path1.AsString() != "/test/path/test.txt")
        return false;
    if (path2.AsString() != "/test/directory/")
        return false;
    if (path3.AsString() != "/test/directory")
        return false;
    if (path4.AsString() != "test/test.txt")
        return false;

    return path1.IsAbsolute() &&
        path2.IsAbsolute() &&
        path3.IsAbsolute() &&
        !path4.IsAbsolute();
}

bool Test_Path_GetFile()
{
    auto file1 = Exi::Runtime::Path("///test//path/test.txt").GetFile();
    auto file2 = Exi::Runtime::Path("///test\\directory\\").GetFile();
    auto file3 = Exi::Runtime::Path("///test\\directory").GetFile();
    auto file4 = Exi::Runtime::Path("test/test.txt").GetFile();

    return file1 == "test.txt" &&
        file2.empty() &&
        file3 == "directory" &&
        file4 == "test.txt";
}

bool Test_Path_GetExtension()
{
    auto ext1 = Exi::Runtime::Path("///test//path/test.txt").GetExtension();
    auto ext2 = Exi::Runtime::Path("///test\\.dotfile").GetExtension();
    auto ext3 = Exi::Runtime::Path("///test\\file.").GetExtension();
    auto ext4 = Exi::Runtime::Path("test/directory").GetExtension();
    auto ext5 = Exi::Runtime::Path("test/test.txt").GetExtension();

    return ext1 == "txt" &&
        ext2 == "dotfile" &&
        ext3.empty() &&
        ext4.empty() &&
        ext5 == "txt";
}

bool Test_Path_GetFileName()
{
    auto ext1 = Exi::Runtime::Path("///test//path/test.txt").GetFileName();
    auto ext2 = Exi::Runtime::Path("///test\\.dotfile").GetFileName();
    auto ext3 = Exi::Runtime::Path("///test\\file.").GetFileName();
    auto ext4 = Exi::Runtime::Path("test/directory").GetFileName();
    auto ext5 = Exi::Runtime::Path("test/test.txt").GetFileName();

    return ext1 == "test" &&
           ext2.empty() &&
           ext3 == "file" &&
           ext4 == "directory" &&
           ext5 == "test";
}

bool Test_Filesystem_MountDirectory()
{
    Exi::Runtime::Filesystem fs;
    std::filesystem::create_directories("Test/1/2/3");

    bool t1 = fs.MountDirectory("Test/1", "/Test1");
    bool t2 = fs.MountDirectory("Test/1/2", "/Test1/Test2");
    bool t3 = fs.MountDirectory("Test/1/2/3", "/Test1/Test2/Test3");
    bool t4 = fs.MountDirectory("Test/1/2/3/4", "/Test1/Test2/Test3/Test4");

    return (t1 && t2 && t3) && !t4;
}

bool Test_Filesystem_TranslatePath()
{
    Exi::Runtime::Filesystem fs;
    std::filesystem::create_directories("Test/1/2/3");

    bool t1 = fs.MountDirectory("Test/1", "/Test1");
    bool t2 = fs.MountDirectory("Test/1/2", "/Test1/Test2");
    bool t3 = fs.MountDirectory("Test/1/2/3", "/Test1/Test2/Test3");

    if (!(t1 && t2 && t3))
        return false;

    Exi::Runtime::Path path1, path2, path3;
    bool xlt1 = fs.TranslatePath("Test1/test.txt", path1);
    bool xlt2 = fs.TranslatePath("Test1/Test2/test.txt", path2);
    bool xlt3 = fs.TranslatePath("Test1/Test2/Test3/test.txt", path3);

    if (!(xlt1 && xlt2 && xlt3))
        return false;

    if (path1 != "Test/1/test.txt")
        return false;

    if (path2 != "Test/1/2/test.txt")
        return false;

    if (path3 != "Test/1/2/3/test.txt")
        return false;

    return true;
}

bool Test_Filesystem_Open()
{
    Exi::Runtime::Filesystem fs;
    FILE* f = fopen("test.txt", "w");
    fprintf(f, "Test!");
    fclose(f);

    auto handle1 = fs.Open("test.txt");
    auto handle2 = fs.Open("test.txt", fs.ReadWrite);

    return handle1 && handle2;
}

bool Test_Threaded_Filesystem_Open()
{
    Exi::Runtime::Filesystem fs;
    FILE* f = fopen("test.txt", "w");
    fprintf(f, "Test!");
    fclose(f);

    const auto n = std::thread::hardware_concurrency();
    assert(n != 0);

    std::vector<std::thread> threads;
    std::array<int, 128> threadStatus = { 0 }; // Each thread only writes to their own assigned slot
    bool status = true;

    for (unsigned int i = 0; i < n; i++)
        threads.emplace_back([&, i]{
            auto handle = fs.Open("test.txt", fs.ReadOnly);
            threadStatus[i] = handle.IsValid();
        });

    for (unsigned int i = 0; i < n; i++)
    {
        threads[i].join();
        if (!threadStatus[i])
            status = false;
    }

    return status;
}

bool Test_FileHandle_Read()
{
    const std::string testString = "Test!";
    Exi::Runtime::Filesystem fs;
    FILE* f = fopen("test.txt", "w");
    fputs(testString.c_str(), f);
    fclose(f);

    auto readHandle = fs.Open("test.txt");
    if (!readHandle)
        return false;

    char c;
    std::string readString;
    while (readHandle.Read(c))
        readString.push_back(c);

    return readString == testString;
}

bool Test_FileHandle_Write()
{
    const std::string testString = "Test!";
    Exi::Runtime::Filesystem fs;

    auto writeHandle = fs.Open("test.txt", Exi::Runtime::Filesystem::WriteTruncate);
    if (!writeHandle)
        return false;
    for (char c : testString)
        if (!writeHandle.Write(c))
            return false;

    auto readHandle = fs.Open("test.txt");
    if (!readHandle)
        return false;

    char c;
    std::string readString;
    while (readHandle.Read(c))
        readString.push_back(c);

    return readString == testString;
}

bool Test_Class_FindMethod()
{
    Exi::Runtime::Class cls("NativeClass", Exi::Runtime::Native);

    auto& m1 = cls.AddMethod("TestMethod1");
    auto& m2 = cls.AddMethod("TestMethod2");

    auto p1 = cls.FindMethod("TestMethod2");
    auto p2 = cls.FindMethod("TestMethod3");

    return (p1 == &m2) && (p2 == nullptr);
}

bool Test_LuaState_Execute()
{
    Exi::Runtime::LuaState lua;
    return lua.Execute("print('Hello from LuaState::Execute()')");
}

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "PathUtils_GetNextFragment", Test_PathUtils_GetNextFragment },
        { "PathUtils_StripSeparators", Test_PathUtils_StripSeparators },
        { "Path_Constructor", Test_Path_Constructor },
        { "Path_GetFile", Test_Path_GetFile },
        { "Path_GetFileName", Test_Path_GetFileName },
        { "Path_GetExtension", Test_Path_GetExtension },
        { "Filesystem_MountDirectory", Test_Filesystem_MountDirectory },
        { "Filesystem_TranslatePath", Test_Filesystem_TranslatePath },
        { "Filesystem_Open", Test_Filesystem_Open },
        { "Threaded_Filesystem_Open", Test_Threaded_Filesystem_Open },
        { "FileHandle_Read", Test_FileHandle_Read },
        { "FileHandle_Write", Test_FileHandle_Write },
        { "Class_FindMethod", Test_Class_FindMethod },
        { "LuaState_Execute", Test_LuaState_Execute },
        { "Benchmark", Benchmark }
    });

    return tests.Execute(argc, argv);
}
