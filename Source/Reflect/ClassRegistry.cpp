#include <Exile/Reflect/Reflection.hpp>

namespace Exi::Reflect
{
    ClassRegistry* ClassRegistry::s_Instance = nullptr;

    ClassRegistry::ClassRegistry()
    {

    }

    ClassRegistry::~ClassRegistry()
    {

    }

    ClassRegistry* ClassRegistry::GetInstance()
    {
        if (!s_Instance)
            s_Instance = new ClassRegistry();
        return s_Instance;
    }

    Class& ClassRegistry::RegisterClass(const Class& clazz, ClassId id)
    {
        auto pair = m_ClassMap.try_emplace(id, clazz);
        return pair.first->second;
    }

    bool ClassRegistry::IsClassRegistered(ClassId id) const
    {
        return m_ClassMap.contains(id);
    }

    const Class* ClassRegistry::GetClass(ClassId id) const
    {
        return m_ClassMap.contains(id) ? &m_ClassMap.at(id) : nullptr;
    }
}
