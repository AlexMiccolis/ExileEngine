#include <Exile/Unit/Test.hpp>
#include <Exile/Runtime/LuaContext.hpp>

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

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "LuaContext_ExecuteString", Test_LuaContext_ExecuteString },
        { "LuaContext_SetGlobalFunction", Test_LuaContext_SetGlobalFunction },
    });

    return tests.Execute(argc, argv);
}
