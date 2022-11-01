#include <Exile/Unit/Test.hpp>
#include <Exile/Runtime/LuaContext.hpp>
#include <Exile/Runtime/Filesystem.hpp>

extern bool Benchmark();

bool Test_LuaContext_ExecuteString()
{
    Exi::Runtime::LuaContext lua;
    return lua.ExecuteString("print('Hello from LuaContext::ExecuteString()')");
}

static int my_global(Exi::Runtime::LuaContext& lua)
{
    lua.Push("Hello from a global Lua function");
    return 1;
}

bool Test_LuaContext_SetGlobalFunction()
{
    Exi::Runtime::LuaContext lua;
    lua.SetGlobalFunction("my_global", my_global);
    return lua.ExecuteString("print(my_global())");
}

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

bool Test_Filesystem_MountDirectory()
{
    Exi::Runtime::Filesystem fs(std::filesystem::current_path());
    std::filesystem::create_directories("Test/1/2/3");

    bool t1 = fs.MountDirectory("Test/1", "/Test1");
    bool t2 = fs.MountDirectory("Test/1/2", "/Test1/Test2");
    bool t3 = fs.MountDirectory("Test/1/2/3", "/Test1/Test2/Test3");
    bool t4 = fs.MountDirectory("Test/1/2/3/4", "/Test1/Test2/Test3/Test4");

    return (t1 && t2 && t3) && !t4;
}

bool Test_Filesystem_TranslatePath()
{
    Exi::Runtime::Filesystem fs(std::filesystem::current_path());
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

    if (path1 != "Test/1/test.txt")
        return false;

    if (path2 != "Test/1/2/test.txt")
        return false;

    if (path3 != "Test/1/2/3/test.txt")
        return false;

    return true;
}

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "LuaContext_ExecuteString", Test_LuaContext_ExecuteString },
        { "LuaContext_SetGlobalFunction", Test_LuaContext_SetGlobalFunction },
        { "PathUtils_GetNextFragment", Test_PathUtils_GetNextFragment },
        { "PathUtils_StripSeparators", Test_PathUtils_StripSeparators },
        { "Filesystem_MountDirectory", Test_Filesystem_MountDirectory },
        { "Filesystem_TranslatePath", Test_Filesystem_TranslatePath },
        { "Benchmark", Benchmark }
    });

    return tests.Execute(argc, argv);
}
