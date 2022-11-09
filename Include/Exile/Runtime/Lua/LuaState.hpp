#pragma once

#include <Exile/Runtime/API.hpp>
#include <functional>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string>

struct lua_State;

namespace Exi::Runtime
{

    template <class T>
    struct RUNTIME_API LuaBinding
    {
        using Owner = T;
        using Fn = int(*)(class LuaState&);

        LuaBinding(Owner* _instance, Fn _function)
            : instance(_instance), function(_function) { }

        T* instance;
        Fn function;
    };

    /** C++ wrapper around a lua_State. Most functions are 1:1 with Lua API calls */
    class RUNTIME_API LuaState
    {
    public:
        using TypeIndex = uint32_t;
        static constexpr TypeIndex InvalidType = -1;

        using CFunction = int(*)(lua_State*);
        using CppFunction = int(*)(LuaState&);

        template <class T>
        using MemberFunction = int(T::*)(LuaState&);

        struct UserDataBlock
        {
            void*     pointer;
            TypeIndex type;
        };

        template <class T>
        struct UserData : public UserDataBlock
        {
            T value;
        };

        LuaState();
        ~LuaState();

        bool Execute(const std::string& code, const std::string& name = "Script");

        void Call(int args = 0, int results = 0);
        bool ProtectedCall(int args = 0, int results = 0, int err = 0);

        void PushNil();
        void PushNumber(double number);
        void PushInteger(ptrdiff_t integer);
        void PushString(const char* string);
        void PushBoolean(bool boolean);
        void PushLightUserdata(void* pointer);
        void PushCClosure(CFunction function, int upvalues = 0);
        void PushCppFunction(CppFunction function);
        void Pop(int n);
        void Remove(int index);

        bool Next(int tableIndex);
        bool Equal(int index1, int index2);
        [[nodiscard]] bool RawEqual(int index1, int index2) const;
        [[nodiscard]] std::size_t ObjectLen(int index) const;

        [[nodiscard]] double         ToNumber(int index);
        [[nodiscard]] ptrdiff_t      ToInteger(int index);
        [[nodiscard]] const char*    ToString(int index);
        [[nodiscard]] bool           ToBoolean(int index);
        [[nodiscard]] const void*    ToPointer(int index);
        [[nodiscard]] UserDataBlock* ToUserdata(int index);
        [[nodiscard]] void*          ToLightUserdata(int index);

        [[nodiscard]] bool IsNil(int index) const;
        [[nodiscard]] bool IsNone(int index) const;
        [[nodiscard]] bool IsNoneOrNil(int index) const;
        [[nodiscard]] bool IsTable(int index) const;
        [[nodiscard]] bool IsNumber(int index) const;
        [[nodiscard]] bool IsString(int index) const;
        [[nodiscard]] bool IsUserdata(int index) const;
        [[nodiscard]] bool IsLightUserdata(int index) const;
        [[nodiscard]] bool IsFunction(int index) const;
        [[nodiscard]] bool IsCFunction(int index) const;

        [[nodiscard]] int GetTop() const;
        void GetGlobal(const char* name);
        void GetTable(int tableIndex);
        void GetTableRaw(int tableIndex);
        void GetTableRawI(int tableIndex, int n);
        void GetField(int tableIndex, const char* key);
        void GetRegistryField(const char* key);
        bool GetMetatable(int tableIndex);

        void SetTop(int index);
        void SetGlobal(const char* name);
        void SetTable(int tableIndex);
        void SetTableRaw(int tableIndex);
        void SetTableRawI(int tableIndex, int n);
        void SetField(int tableIndex, const char* key);
        void SetRegistryField(const char* key);
        bool SetMetatable(int tableIndex);

        bool Contains(int tableIndex, const char* key);

        void CreateTable();
        void* CreateUserdata(std::size_t size);

    private:
        lua_State*    m_Lua;
        class Logger& m_Logger;

        static int CppWrapper(lua_State* lua);
    };

}
