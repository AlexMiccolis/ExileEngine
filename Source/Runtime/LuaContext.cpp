#include <Exile/Runtime/LuaContext.hpp>
#include <Exile/Runtime/Logger.hpp>

namespace Exi::Runtime
{
    static int BytecodeVectorWriter(lua_State *L, const void* p, size_t sz, void* ud)
    {
        auto* bytecode = static_cast<std::vector<uint8_t>*>(ud);
        bytecode->reserve(bytecode->size() + sz);
        for (std::size_t i = 0; i < sz; i++)
            bytecode->emplace_back(static_cast<const uint8_t*>(p)[i]);
        return 0;
    }

    LuaContext::LuaContext()
    {
        m_Lua = luaL_newstate();
        luaL_openlibs(m_Lua);
    }

    LuaContext::~LuaContext()
    {
        if (m_Lua != nullptr)
            lua_close(m_Lua);
    }

    bool LuaContext::ExecuteString(const std::string& str)
    {
        if (luaL_dostring(m_Lua, str.c_str()))
        {
            const char* err = lua_tostring(m_Lua, -1);
            Logger::GetLogger("Lua").Error(err);
            return false;
        }
        return true;
    }

    bool LuaContext::CompileString(const std::string& str, std::vector<uint8_t>& bytecodeOut)
    {
        if (luaL_loadstring(m_Lua, str.c_str()))
        {
            const char* err = lua_tostring(m_Lua, -1);
            Logger::GetLogger("Lua").Error(err);
            return false;
        }
        return lua_dump(m_Lua, BytecodeVectorWriter, &bytecodeOut) == 0;
    }

    bool LuaContext::CompileFile(const std::string& path, std::vector<uint8_t>& bytecodeOut)
    {
        if (luaL_loadfile(m_Lua, path.c_str()))
        {
            const char* err = lua_tostring(m_Lua, -1);
            Logger::GetLogger("Lua").Error(err);
            return false;
        }
        return lua_dump(m_Lua, BytecodeVectorWriter, &bytecodeOut) == 0;
    }

    bool LuaContext::SetGlobalFunction(const std::string& name, LuaFn fn)
    {
        // Creates a closure that calls LuaFnWrapper with
        // this context and fn as arguments.
        lua_pushlightuserdata(m_Lua, this);
        lua_pushlightuserdata(m_Lua, reinterpret_cast<void*>(fn));
        lua_pushcclosure(m_Lua, LuaFnWrapper, 2);
        lua_setglobal(m_Lua, name.c_str());
        return true;
    }


    void LuaContext::Push(std::nullptr_t) { lua_pushnil(m_Lua); }
    void LuaContext::Push(int value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(float value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(const double& value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(const char* value) { lua_pushstring(m_Lua, value); }
    void LuaContext::Push(const std::string& value) { lua_pushstring(m_Lua, value.c_str()); }


    int LuaContext::LuaFnWrapper(lua_State* lua)
    {
        LuaContext* context = reinterpret_cast<LuaContext*>(lua_touserdata(lua, lua_upvalueindex(1)));
        LuaFn fn = reinterpret_cast<LuaFn>(lua_touserdata(lua, lua_upvalueindex(2)));
        return fn(*context);
    }

}
