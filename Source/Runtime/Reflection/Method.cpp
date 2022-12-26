#include <Exile/Runtime/Reflection/Class.hpp>
#include <Exile/Runtime/Reflection/Method.hpp>

namespace Exi::Runtime
{

    Method::Method(const Class& owner, std::string name)
        : m_Owner(owner), m_Name(std::move(name))
    {

    }

    void Method::SetPointer(void* ptr)
    {
        m_Function = ptr;
    }

    void Method::SetSignature(Type returnType)
    {
        m_ReturnType = returnType;
    }

    void Method::SetSignature(Type returnType, int parameters, ParameterTypes parameterTypes)
    {
        m_ReturnType = returnType;
        m_Parameters = parameters;
        m_ParameterTypes = parameterTypes;
    }

}
