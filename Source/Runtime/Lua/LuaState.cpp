#include "Exile/Runtime/Lua/LuaState.hpp"
#include <Exile/Runtime/Logger.hpp>
#include <lua.hpp>

namespace Exi::Runtime
{

    static const luaL_Reg s_LuaLibs[] = {
            { "",               luaopen_base },
            { LUA_TABLIBNAME,   luaopen_table },
            { LUA_STRLIBNAME,   luaopen_string },
            { LUA_MATHLIBNAME,  luaopen_math },
            { LUA_DBLIBNAME,    luaopen_debug },
            { LUA_BITLIBNAME,   luaopen_bit },
            { LUA_JITLIBNAME,   luaopen_jit }
    };

    LuaState::LuaState()
        : m_Logger(Logger::GetLogger("Lua"))
    {
        m_Lua = luaL_newstate();

        // Open built-in libraries
        for (auto s_LuaLib : s_LuaLibs)
        {
            PushCClosure(s_LuaLib.func);
            PushString(s_LuaLib.name);
            Call(1);
        }

    }

    LuaState::~LuaState()
    {
        if (m_Lua)
        {
            lua_close(m_Lua);
            m_Lua = nullptr;
        }
    }



    bool LuaState::Execute(const std::string& code, const std::string& name)
    {
        if (luaL_loadbuffer(m_Lua, code.data(), code.size(), name.c_str()) != 0
            || !ProtectedCall())
        {
            const char* err = ToString(GetTop());
            m_Logger.Error(err);
            return false;
        }

        return true;
    }



    void LuaState::Call(int args, int results)
    {
        lua_call(m_Lua, args, results);
    }

    bool LuaState::ProtectedCall(int args, int results, int err)
    {
        return lua_pcall(m_Lua, args, results, err) == 0;
    }



    void LuaState::PushNil()
    {
        lua_pushnil(m_Lua);
    }

    void LuaState::PushNumber(double number)
    {
        lua_pushnumber(m_Lua, number);
    }

    void LuaState::PushInteger(ptrdiff_t integer)
    {
        lua_pushinteger(m_Lua, integer);
    }

    void LuaState::PushString(const char* string)
    {
        lua_pushstring(m_Lua, string);
    }

    void LuaState::PushBoolean(bool boolean)
    {
        lua_pushboolean(m_Lua, boolean);
    }

    void LuaState::PushLightUserdata(void* pointer)
    {
        lua_pushlightuserdata(m_Lua, pointer);
    }

    void LuaState::PushCClosure(LuaState::CFunction function, int upvalues)
    {
        lua_pushcclosure(m_Lua, function, upvalues);
    }

    void LuaState::PushCppFunction(LuaState::CppFunction function)
    {
        lua_pushlightuserdata(m_Lua, this);
        lua_pushlightuserdata(m_Lua, reinterpret_cast<void*>(function));
        PushCClosure(CppWrapper, 2);
    }

    void LuaState::Pop(int n)
    {
        lua_pop(m_Lua, n);
    }

    void LuaState::Remove(int index)
    {
        lua_remove(m_Lua, index);
    }



    bool LuaState::Next(int tableIndex)
    {
        return lua_next(m_Lua, tableIndex);
    }

    bool LuaState::Equal(int index1, int index2)
    {
        return lua_equal(m_Lua, index1, index2);
    }

    bool LuaState::RawEqual(int index1, int index2) const
    {
        return lua_rawequal(m_Lua, index1, index2);
    }

    std::size_t LuaState::ObjectLen(int index) const
    {
        return lua_objlen(m_Lua, index);
    }



    double LuaState::ToNumber(int index)
    {
        return lua_tonumber(m_Lua, index);
    }

    ptrdiff_t LuaState::ToInteger(int index)
    {
        return lua_tointeger(m_Lua, index);
    }

    const char* LuaState::ToString(int index)
    {
        return lua_tostring(m_Lua, index);
    }

    bool LuaState::ToBoolean(int index)
    {
        return lua_toboolean(m_Lua, index);
    }

    const void* LuaState::ToPointer(int index)
    {
        return lua_topointer(m_Lua, index);
    }

    void* LuaState::ToLightUserdata(int index)
    {
        return lua_touserdata(m_Lua, index);
    }



    bool LuaState::IsNil(int index) const
    {
        return lua_isnil(m_Lua, index);
    }

    bool LuaState::IsNone(int index) const
    {
        return lua_isnone(m_Lua, index);
    }

    bool LuaState::IsNoneOrNil(int index) const
    {
        return lua_isnoneornil(m_Lua, index);
    }

    bool LuaState::IsTable(int index) const
    {
        return lua_istable(m_Lua, index);
    }

    bool LuaState::IsNumber(int index) const
    {
        return lua_isnumber(m_Lua, index);
    }

    bool LuaState::IsString(int index) const
    {
        return lua_isstring(m_Lua, index);
    }

    bool LuaState::IsUserdata(int index) const
    {
        return lua_isuserdata(m_Lua, index);
    }

    bool LuaState::IsLightUserdata(int index) const
    {
        return lua_islightuserdata(m_Lua, index);
    }

    bool LuaState::IsFunction(int index) const
    {
        return lua_isfunction(m_Lua, index);
    }

    bool LuaState::IsCFunction(int index) const
    {
        return lua_iscfunction(m_Lua, index);
    }



    int LuaState::GetTop() const
    {
        return lua_gettop(m_Lua);
    }

    void LuaState::GetTable(int tableIndex)
    {
        lua_gettable(m_Lua, tableIndex);
    }

    void LuaState::GetTableRaw(int tableIndex)
    {
        lua_rawget(m_Lua, tableIndex);
    }

    void LuaState::GetTableRawI(int tableIndex, int n)
    {
        lua_rawgeti(m_Lua, tableIndex, n);
    }

    void LuaState::GetGlobal(const char* name)
    {
        lua_getglobal(m_Lua, name);
    }

    void LuaState::GetField(int tableIndex, const char* key)
    {
        lua_getfield(m_Lua, tableIndex, key);
    }

    void LuaState::GetRegistryField(const char* key)
    {
        lua_getfield(m_Lua, LUA_REGISTRYINDEX, key);
    }

    bool LuaState::GetMetatable(int tableIndex)
    {
        return lua_getmetatable(m_Lua, tableIndex);
    }



    void LuaState::SetTop(int index)
    {
        lua_settop(m_Lua, index);
    }

    void LuaState::SetGlobal(const char* name)
    {
        lua_setglobal(m_Lua, name);
    }

    void LuaState::SetTable(int tableIndex)
    {
        lua_settable(m_Lua, tableIndex);
    }

    void LuaState::SetTableRaw(int tableIndex)
    {
        lua_rawset(m_Lua, tableIndex);
    }

    void LuaState::SetTableRawI(int tableIndex, int n)
    {
        lua_rawseti(m_Lua, tableIndex, n);
    }


    void LuaState::SetField(int tableIndex, const char* key)
    {
        lua_setfield(m_Lua, tableIndex, key);
    }

    void LuaState::SetRegistryField(const char* key)
    {
        lua_setfield(m_Lua, LUA_REGISTRYINDEX, key);
    }

    bool LuaState::SetMetatable(int tableIndex)
    {
        return lua_setmetatable(m_Lua, tableIndex);
    }



    bool LuaState::Contains(int tableIndex, const char* key)
    {
        bool result = true;

        PushString(key);
        GetTableRaw(tableIndex);

        if (IsNil(GetTop()))
            result = false;

        Pop(1);
        return result;
    }



    void LuaState::CreateTable()
    {
        lua_newtable(m_Lua);
    }

    void* LuaState::CreateUserdata(std::size_t size)
    {
        return lua_newuserdata(m_Lua, size);
    }



    int LuaState::CppWrapper(lua_State* lua)
    {
        auto* context = reinterpret_cast<LuaState*>(lua_touserdata(lua, lua_upvalueindex(1)));
        auto  fn      = reinterpret_cast<CppFunction>(lua_touserdata(lua, lua_upvalueindex(2)));
        return fn(*context);
    }

}
