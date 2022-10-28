#pragma once

#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/System.hpp>
#include <Exile/Reflect/Reflection.hpp>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <vector>

namespace Exi::ECS
{

    /**
     * The entity manager is responsible for keeping track of all entities and
     * distributing them to systems.
     */
    class EntityManager
    {
    public:
        using EntityId = uint32_t;
        using SystemId = uint32_t;

        EntityManager();
        ~EntityManager();

        /**
         * Register a system with the entity manager
         * @param system
         * @return
         */
        SystemId RegisterSystem(System* system);

        /**
         * Run ticks for all registered systems
         * @param deltaTime
         */
        void TickSystems(double deltaTime);

        /**
         * Add an entity to this entity manager
         * @param entity
         * @returns Entity ID
         */
        EntityId AddEntity(std::unique_ptr<Entity>&& entity);

        /**
         * Get an entity by its ID
         * @param id
         * @return Entity pointer if found, nullptr otherwise
         */
        const Entity* GetEntity(EntityId id) const;

        /**
         * Find all entities with one or more of the specified component
         * @param component
         * @param entitiesOut
         * @return Number of entities with the specified component
         */
        int GetEntitiesWithComponent(Reflect::ClassId component, std::vector<const Entity*>& entitiesOut) const;
    private:
        mutable std::shared_mutex m_Mutex;
        std::vector<System*> m_Systems;
        std::vector<std::unique_ptr<Entity>> m_Entities;
    };

}