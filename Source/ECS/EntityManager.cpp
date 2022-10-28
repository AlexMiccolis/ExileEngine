#include <Exile/ECS/EntityManager.hpp>

namespace Exi::ECS
{

    EntityManager::EntityManager()
    {

    }

    EntityManager::~EntityManager()
    {

    }

    EntityManager::SystemId EntityManager::RegisterSystem(System* system)
    {
        std::unique_lock lock(m_Mutex);
        SystemId id = m_Systems.size();
        m_Systems.emplace_back(system);

        if (m_Entities.size() > 0)
        {
            // New system needs to be notified of existing entities
            for (auto& pair : m_Entities)
            {
                Entity& e = *pair.second.get();
                if (system->NotifyEntity(e))
                    system->AddEntity(e);
            }
        }

        return id;
    }

    void EntityManager::TickSystems(double deltaTime)
    {
        std::shared_lock lock(m_Mutex);
        for (auto* system : m_Systems)
        {
            system->Tick(deltaTime);
        }
    }

    EntityManager::EntityId EntityManager::AddEntity(std::unique_ptr<Entity>&& entity)
    {
        std::unique_lock lock(m_Mutex);

        EntityId id = TL::UUID::Random();
        auto pair = m_Entities.emplace(id, std::move(entity));
        auto& e = *pair.first->second.get();

        e.SetUniqueId(id);
        e.SetEntityManager(this);

        for (auto* system : m_Systems)
        {
            if (system->NotifyEntity(e))
                system->AddEntity(e);
        }

        return id;
    }

    const Entity* EntityManager::GetEntity(EntityManager::EntityId id) const
    {
        std::shared_lock lock(m_Mutex);
        if (!m_Entities.contains(id))
            return nullptr;
        return m_Entities.at(id).get();
    }

    Entity* EntityManager::RemoveEntity(EntityManager::EntityId id)
    {
        std::unique_lock lock(m_Mutex);
        if (!m_Entities.contains(id))
            return nullptr;

        // Release entity from smart pointer
        Entity* entity = m_Entities.at(id).release();

        // Notify systems that it is being removed
        for (auto* system : m_Systems)
            system->NotifyEntityRemoved(*entity);

        // Erase it from the map
        m_Entities.erase(id);
        return entity;
    }

}
