#include <Exile/Reflect/Reflection.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

namespace Exi::ECS
{
    Entity::Entity(const std::string_view& name)
            : m_RootComponent(nullptr), m_Name(name)
    {

    }

    Entity::~Entity()
    {

    }

    Component* Entity::GetComponent(Reflect::ClassId id) const
    {
        Component* component = nullptr;
        m_ComponentMap.Find(id, &component, 1);
        return component;
    }

    int Entity::GetComponentsOfType(Reflect::ClassId id, Component** components, std::size_t maxComponents) const
    {
        return m_ComponentMap.Find(id, components, maxComponents);
    }

    void Entity::AttachComponent(Reflect::ClassId id, Component* component)
    {
        m_ComponentMap.Emplace(id, component)->OnAttached(*this);
    }

    int Entity::GetComponentCount(Reflect::ClassId id) const
    {
        return m_ComponentMap.Count(id);
    }

}