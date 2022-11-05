#include "Exile/Runtime/LuaContext.hpp"
#include "Exile/Runtime/Filesystem.hpp"
#include "Exile/Runtime/Logger.hpp"

namespace Exi::Runtime
{
    /**
     * Helper struct to read buffers of source code or bytecode
     */
    struct LuaBufferReader
    {
        const char* data;
        std::size_t size;
        bool consumed;

        explicit LuaBufferReader(const char* _data, std::size_t _size)
            : data(_data), size(_size), consumed(false) { }
    };

    static int BytecodeVectorWriter(lua_State *L, const void* p, size_t sz, void* ud)
    {
        auto* bytecode = static_cast<std::vector<uint8_t>*>(ud);
        bytecode->reserve(bytecode->size() + sz);
        for (std::size_t i = 0; i < sz; i++)
            bytecode->emplace_back(static_cast<const uint8_t*>(p)[i]);
        return 0;
    }

    static const char* BufferReader(lua_State *L, void *ud, size_t *size)
    {
        auto* reader = static_cast<LuaBufferReader*>(ud);
        if (reader->consumed)
        {
            *size = 0;
            return nullptr;
        }

        reader->consumed = true;
        *size = reader->size;
        return reinterpret_cast<const char*>(reader->data);
    }

    LuaContext::LuaContext(Filesystem& filesystem)
        : m_Filesystem(filesystem)
    {
        m_Lua = luaL_newstate();
        luaL_openlibs(m_Lua);
        PopulateApiTables();
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

    bool LuaContext::ExecuteBytecode(const std::vector<uint8_t>& bytecode, const std::string& chunkName)
    {
        LuaBufferReader ctx(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
        auto status = lua_load(m_Lua, BufferReader, &ctx, chunkName.c_str());
        if (status != 0)
        {
            Logger::GetLogger("Lua").Error("Failed to load bytecode buffer '%s'", chunkName.c_str());
            return false;
        }

        status = lua_pcall(m_Lua, 0, LUA_MULTRET, 0);
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
        auto file = m_Filesystem.Open(path);

        if (!file)
            return false;

        auto str = file.ReadString();

        LuaBufferReader ctx(reinterpret_cast<const char*>(str.data()), str.length());
        auto status = lua_load(m_Lua, BufferReader, &ctx, path.c_str());
        if (status != 0)
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
        Push(fn);
        lua_setglobal(m_Lua, name.c_str());
        return true;
    }


    void LuaContext::Push(std::nullptr_t) { lua_pushnil(m_Lua); }
    void LuaContext::Push(int value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(float value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(const double& value) { lua_pushnumber(m_Lua, value); }
    void LuaContext::Push(const char* value) { lua_pushstring(m_Lua, value); }
    void LuaContext::Push(const std::string& value) { lua_pushstring(m_Lua, value.c_str()); }
    void LuaContext::Push(LuaContext::LuaFn value)
    {
        lua_pushlightuserdata(m_Lua, this);
        lua_pushlightuserdata(m_Lua, reinterpret_cast<void*>(value));
        lua_pushcclosure(m_Lua, LuaFnWrapper, 2);
    }

    LuaContext::TableIndex LuaContext::NewTable()
    {
        lua_newtable(m_Lua);
        return lua_gettop(m_Lua);
    }

    void LuaContext::SetField(TableIndex table, const std::string& name)
    {
        lua_setfield(m_Lua, table, name.c_str());
    }

    std::size_t LuaContext::GetMemory() const
    {
        return lua_gc(m_Lua, LUA_GCCOUNT, 0);
    }

    void LuaContext::StepGc()
    {
        lua_gc(m_Lua, LUA_GCSTEP, 1);
    }

    void LuaContext::CollectGc()
    {
        lua_gc(m_Lua, LUA_GCCOLLECT, 0);
    }

    void LuaContext::PopulateApiTables()
    {
        auto Exi = NewTable();

        // Exi.Class
        SetField(Exi, "Class", ExiClass);

        lua_setglobal(m_Lua, ApiTable);
    }

    int LuaContext::ExiClass(LuaContext& lua)
    {
        return 0;
    }

    int LuaContext::LuaFnWrapper(lua_State* lua)
    {
        auto* context = reinterpret_cast<LuaContext*>(lua_touserdata(lua, lua_upvalueindex(1)));
        auto fn = reinterpret_cast<LuaFn>(lua_touserdata(lua, lua_upvalueindex(2)));
        return fn(*context);
    }

}
