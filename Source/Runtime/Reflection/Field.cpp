#include <Exile/Runtime/Reflection/Class.hpp>
#include <Exile/Runtime/Reflection/Field.hpp>

namespace Exi::Runtime
{

    Field::Field(const Class& owner, std::string name, Type type)
        : m_Owner(owner), m_Name(std::move(name)), m_Type(type)
    {

    }

    void Field::SetOffset(std::size_t offset)
    {
        m_Offset = offset;
    }

}
