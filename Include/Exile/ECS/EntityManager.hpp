#pragma once

#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/System.hpp>
#include <Exile/TL/UUID.hpp>
#include <Exile/Reflect/Reflection.hpp>
#include <shared_mutex>
#include <mutex>
#include <memory>
#include <vector>
#include <unordered_map>

namespace Exi::ECS
{

    /**
     * The entity manager is responsible for keeping track of all entities and
     * distributing them to systems.
     */
    class EntityManager
    {
    public:
        using EntityId = TL::UUID;
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

    private:
        /**
         * Remove an entity and return its raw pointer
         * @param id
         * @return Entity pointer if it exists, nullptr otherwise
         */
        Entity* RemoveEntity(EntityId id);

        mutable std::shared_mutex m_Mutex;
        std::vector<System*> m_Systems;
        std::unordered_map<EntityId, std::unique_ptr<Entity>> m_Entities;
        // TODO: Use something faster than unordered_map
    };

}