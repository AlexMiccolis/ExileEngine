#pragma once

#include <Exile/ECS/Entity.hpp>
#include <vector>

namespace Exi::ECS
{

    DefineClass(System)
    {
    public:
        System() = default;
        virtual ~System() = default;

        /**
         * Tick the system
         * @param deltaTime Time in seconds since last tick
         */
        virtual void Tick(double deltaTime);

        /**
         * Called by the entity manager to notify the system of a new entity.
         * @param entity
         * @return Returns true if the entity is usable by the system, false otherwise
         */
        virtual bool NotifyEntity(const Entity& entity);

        /**
         * Called by the entity manager to notify the system that
         * an entity is being removed.
         * @param entity
         * @return Returns true if this system was using the entity, false otherwise
         */
        virtual bool NotifyEntityRemoved(Entity& entity);

        /**
         * Add an entity to the system.
         * @param entity
         */
        virtual void AddEntity(Entity& entity);

        [[nodiscard]] const std::vector<Entity*>& GetEntities() const { return m_Entities; }
    protected:
        /**
         * Entities referenced by this system.
         * The system does NOT have ownership over these entities.
         */
        std::vector<Entity*> m_Entities;
    };

}