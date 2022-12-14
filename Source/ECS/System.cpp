#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/System.hpp>
#include <Exile/Reflect/Reflection.hpp>
#include <cstdio>

namespace Exi::ECS
{

    /**
     * Default Tick implementation, does absolutely nothing
     * @param deltaTime
     */
    void System::Tick(double deltaTime)
    {

    }

    /**
     * Default NotifyEntity implementation, just returns false for everything
     * @param entity
     * @return false
     */
    bool System::NotifyEntity(const Entity& entity)
    {
        return false;
    }

    /**
     * Default NotifyEntityRemoved implementation, always returns false
     * @param entity
     * @return false
     */
    bool System::NotifyEntityRemoved(Entity& entity)
    {
        return false;
    }

    /**
     * AddEntity implementation, must be called with Super::AddEntity if overridden
     * @param entity
     */
    void System::AddEntity(Entity& entity)
    {
        m_Entities.push_back(&entity);
    }

}
