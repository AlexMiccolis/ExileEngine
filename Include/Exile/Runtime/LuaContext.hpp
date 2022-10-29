#pragma once

#include <Exile/Runtime/API.hpp>
#include <lua.hpp>
#include <string>
#include <vector>

namespace Exi::Runtime
{

    class RUNTIME_API LuaContext
    {
    public:
        using LuaFn = int(*)(LuaContext&);

        LuaContext();
        ~LuaContext();

    #pragma region Code Import/Export

        /**
         * Execute a string of Lua code
         * @param str
         * @return True if successful, false otherwise
         */
        bool ExecuteString(const std::string& str);

        /**
         * Compile a string of Lua code into a vector of bytecode
         * @param str
         * @param bytecodeOut
         * @return True if successful, false otherwise
         */
        bool CompileString(const std::string& str, std::vector<uint8_t>& bytecodeOut);

        /**
         * Compile a Lua file into a vector of bytecode
         * @param path
         * @param bytecodeOut
         * @return True if successful, false otherwise
         */
        bool CompileFile(const std::string& path, std::vector<uint8_t>& bytecodeOut);

    #pragma endregion

    #pragma region Global Manipulation

        /**
         * Add a function to the Lua environment as a global
         * @param name
         * @param fn
         * @return True if successful, false otherwise
         */
        bool SetGlobalFunction(const std::string& name, LuaFn fn);

    #pragma endregion

    #pragma region Stack Manipulation

        void Push(std::nullptr_t);
        void Push(int value);
        void Push(float value);
        void Push(const double& value);
        void Push(const char* value);
        void Push(const std::string& value);

    #pragma endregion

    private:
        static int LuaFnWrapper(lua_State* lua);

        lua_State* m_Lua;
    };

}
