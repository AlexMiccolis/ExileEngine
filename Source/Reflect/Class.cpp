#include <Exile/Reflect/Reflection.hpp>

namespace Exi::Reflect
{

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