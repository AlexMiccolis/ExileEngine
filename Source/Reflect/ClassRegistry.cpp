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

    void ClassRegistry::DumpClass(ClassId id) const
    {
        const Class* theClass = GetClass(id);
        const Class* superClass;
        if (!theClass)
        {
            printf("Class ID '%llx' was not found!\n", id);
            return;
        }

        superClass = GetClass(theClass->GetSuperId());
        if (!superClass)
            printf("\nClass %s (ID: %llx):\n", theClass->GetName(), theClass->GetId());
        else
            printf("\nClass %s (ID: %llx): extends %s (ID: %llx)\n", theClass->GetName(), theClass->GetId(),
                   superClass->GetName(), superClass->GetId());

        auto methodCount = theClass->GetMethodCount();
        auto fieldCount  = theClass->GetFieldCount();
        auto methods = new Method const*[methodCount];
        auto fields  = new Field const*[fieldCount];

        methodCount = theClass->GetMethods(methods, methodCount);
        fieldCount  = theClass->GetFields(fields, fieldCount);

        printf("    Methods:\n");
        for (auto i = 0; i < methodCount; i++)
        {

        }

        printf("    Fields:\n");
        for (auto i = 0; i < fieldCount; i++)
        {
            const Field* field = fields[i];
            TL::Type type = field->GetType();
            const auto& typeName = type > TL::TypeEND ? "object" : TL::TypeNames[type];
            printf("        %s : %s\n", field->GetName(), typeName.data());
        }

        delete [] fields;
        delete [] methods;
    }
}
