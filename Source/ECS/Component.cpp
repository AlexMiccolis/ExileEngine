#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

namespace Exi::ECS
{

    Component::Component()
        : m_Entity(nullptr)
    {

    }

    void Component::OnAttached(Entity& entity)
    {
        m_Entity = &entity;
    }

    void Component::StaticInitialize(Reflect::Class& Class)
    {
        ExposeField(Class, m_Entity);
    }

}