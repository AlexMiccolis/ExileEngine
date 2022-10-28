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
        m_Systems.push_back(system);
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
        EntityId id = m_Entities.size();
        auto* e = m_Entities.emplace_back(std::move(entity)).get();
        for (auto* system : m_Systems)
        {
            if (system->NotifyEntity(*e))
                system->AddEntity(e);
        }
        return id;
    }

    const Entity* EntityManager::GetEntity(EntityManager::EntityId id) const
    {
        std::shared_lock lock(m_Mutex);
        if (id >= m_Entities.size())
            return nullptr;
        return m_Entities[id].get();
    }

    int EntityManager::GetEntitiesWithComponent(Reflect::ClassId component, std::vector<const Entity*>& entitiesOut) const
    {
        std::shared_lock lock(m_Mutex);
        int found = 0;
        for (const auto& entity : m_Entities)
        {
            if (entity->GetComponentCount(component))
            {
                entitiesOut.push_back(entity.get());
                ++found;
            }
        }
        return found;
    }

}