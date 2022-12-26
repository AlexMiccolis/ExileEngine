#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Reflection/Types.hpp>
#include <string>
#include <array>

namespace Exi::Runtime
{
    class Class;

    class RUNTIME_API Method
    {
    public:
        static constexpr int MaxParameters = 16;
        using ParameterTypes = std::array<uint8_t, MaxParameters>;

        EXI_NO_COPY(Method);
        Method(const Class& owner, std::string name);

        [[nodiscard]] const Class& GetOwner() const { return m_Owner; }
        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] Type GetReturnType() const { return m_ReturnType; }
        [[nodiscard]] int GetParameters() const { return m_Parameters; }
        [[nodiscard]] const ParameterTypes& GetParameterTypes() const { return m_ParameterTypes; }

        void SetPointer(void* ptr);
        void SetSignature(Type returnType);
        void SetSignature(Type returnType, int parameters, ParameterTypes parameterTypes);
    private:
        const Class& m_Owner;
        const std::string m_Name;

        Type m_ReturnType = Type::TypeNull;
        int m_Parameters  = 0;
        ParameterTypes m_ParameterTypes = { };

        void* m_Function = nullptr;
    };

}
