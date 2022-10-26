#include <Exile/Reflect/Reflection.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

namespace Exi::ECS
{
    Entity::Entity()
        : m_RootComponent(nullptr)
    {

    }

    Entity::~Entity()
    {
        for (auto& pair : m_ComponentMap)
        {
            delete pair.second;
        }
    }

    Component* Entity::GetComponent(Reflect::ClassId id) const
    {
        auto it = m_ComponentMap.find(id);
        return it != m_ComponentMap.end() ? it->second : nullptr;
    }

    int Entity::GetComponentsOfType(Reflect::ClassId id, Component** components, std::size_t maxComponents) const
    {
        auto iterators = m_ComponentMap.equal_range(id);
        int count = 0;
        for (auto [begin, end] = iterators; begin != end; ++begin)
        {
            if (count >= maxComponents)
                break;
            *(components++) = begin->second;
            ++count;
        }
        return count;
    }

    void Entity::AttachComponent(Reflect::ClassId id, Component* component)
    {
        m_ComponentMap.emplace(id, component)->second->OnAttached(*this);
    }

    int Entity::GetComponentCount(Reflect::ClassId id) const
    {
        return m_ComponentMap.count(id);
    }

}