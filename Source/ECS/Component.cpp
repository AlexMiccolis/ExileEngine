#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

namespace Exi::ECS
{

    Component::Component()
    {

    }

    void Component::AttachTo(class Entity& entity)
    {

    }

    void Component::StaticInitialize(Reflect::Class& Class)
    {
        ExposeField(Class, m_Entity);
    }

}