#include <Exile/Runtime/Reflection/Class.hpp>
#include <algorithm>

namespace Exi::Runtime
{

    Class::Class(std::string name, Realm realm)
        : m_Name(std::move(name)), m_UniqueId(TL::UUID::Random()), m_Realm(realm)
    {

    }

    Method& Class::AddMethod(const std::string& name)
    {
        return m_Methods.emplace_back(*this, name);
    }

    Field& Class::AddField(const std::string& name, Type type)
    {
        return m_Fields.emplace_back(*this, name, type);
    }

    const Method* Class::FindMethod(const std::string& name) const
    {
        auto it = std::find_if(m_Methods.cbegin(), m_Methods.cend(),
            [&](const Method& method)
            {
                return method.GetName() == name;
            }
        );

        if (it == m_Methods.cend())
            return nullptr;

        return &(*it);
    }

    const Field* Class::FindField(const std::string& name) const
    {
        auto it = std::find_if(m_Fields.cbegin(), m_Fields.cend(),
            [&](const Field& field)
            {
                return field.GetName() == name;
            }
        );

        if (it == m_Fields.cend())
            return nullptr;

        return &(*it);
    }

}
