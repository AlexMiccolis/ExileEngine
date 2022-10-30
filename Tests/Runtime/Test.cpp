#include <iostream>
#include <Exile/Unit/Test.hpp>
#include <Exile/Runtime/LuaContext.hpp>
#include <Exile/Runtime/Filesystem.hpp>

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

bool Test_PathUtils_GetFirstFragment()
{
    const std::string path = "/\\a////b\\/c/";
    std::size_t index = 0;
    auto fragment = Exi::Runtime::PathUtils::GetFirstFragment(path, index);
    std::vector<std::string_view> fragments;

    while (!fragment.empty())
    {
        fragments.push_back(fragment);
        fragment = Exi::Runtime::PathUtils::GetFirstFragment(path, index);
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

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "LuaContext_ExecuteString", Test_LuaContext_ExecuteString },
        { "LuaContext_SetGlobalFunction", Test_LuaContext_SetGlobalFunction },
        { "PathUtils_GetFirstFragment", Test_PathUtils_GetFirstFragment },
        { "PathUtils_StripSeparators", Test_PathUtils_StripSeparators }
    });

    Exi::Runtime::Filesystem fs(std::filesystem::current_path());

    Exi::Runtime::Path translated;
    if (fs.TranslatePath("Entities/Objects/kekker.lua", translated))
    {
        std::cout << translated << std::endl;
    }
    else
    {
        std::cout << "Translation failed" << std::endl;
    }

    return tests.Execute(argc, argv);
}
