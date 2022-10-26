#include <Exile/Reflect/Reflection.hpp>

namespace Exi::Reflect
{

    std::size_t Class::GetFields(Field const** fields, std::size_t maxFields) const
    {
        std::size_t count = 0;
        for (const auto& pair : m_FieldMap)
        {
            if (maxFields-- == 0)
                break;
            fields[count++] = &pair.second;
        }
        return count;
    }

    std::size_t Class::GetMethods(Method const** methods, std::size_t maxMethods) const
    {
        std::size_t count = 0;
        for (const auto& pair : m_MethodMap)
        {
            if (maxMethods-- == 0)
                break;
            methods[count++] = &pair.second;
        }
        return count;
    }

    const Field* Class::GetInheritedField(FieldId id) const
    {
        if (m_SuperId != 0)
        {
            auto* Registry = ClassRegistry::GetInstance();
            auto* Super = Registry->GetClass(m_SuperId);

            if (!Super)
                return nullptr;

            return Super->GetField(id);
        }
        return nullptr;
    }

    const Method* Class::GetInheritedMethod(MethodId id) const
    {
        if (m_SuperId != 0)
        {
            auto* Registry = ClassRegistry::GetInstance();
            auto* Super = Registry->GetClass(m_SuperId);

            if (!Super)
                return nullptr;

            return Super->GetMethod(id);
        }
        return nullptr;
    }

}