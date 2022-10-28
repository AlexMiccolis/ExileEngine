#pragma once

#include <Exile/Reflect/Reflection.hpp>
#include <Exile/TL/NumericMap.hpp>
#include <Exile/TL/UUID.hpp>
#include <memory>

namespace Exi::ECS
{
    class Component;
    class EntityManager;

    DefineClass(Entity)
    {
    public:
        Entity(const std::string_view& name = "");
        virtual ~Entity();

        /**
         * Get the first component with the given type
         * @param id
         * @return Pointer to component if found, null otherwise
         */
        Component* GetComponent(Reflect::ClassId id) const;

        /**
         * Get the first component with the given type
         * @param id
         * @return Pointer to component if found, null otherwise
         */
        template <Reflect::ReflectiveClass C> requires std::derived_from<C, Component>
        C* GetComponent() const
        {
            return static_cast<C*>(GetComponent(C::Static::Id));
        }

        /**
         * Fill an array with pointers to components of a given type
         * @param id
         * @param components
         * @param maxComponents
         * @return Number of components found
         */
        int GetComponentsOfType(Reflect::ClassId id, Component** components, std::size_t maxComponents) const;

        /**
         * Fill an array with pointers to components of a given type
         * @param id
         * @param components
         * @param maxComponents
         * @return Number of components found
         */
        template <Reflect::ReflectiveClass C> requires std::derived_from<C, Component>
        int GetComponentsOfType(std::vector<C*>& components) const
        {
            components.resize(GetComponentCount<C>());
            return GetComponentsOfType(C::Static::Id, reinterpret_cast<Component**>(components.data()), components.size());
        }

        /**
         * Attach a component to this entity, given a class ID and instance
         * @param id
         * @param component
         */
        void AttachComponent(Reflect::ClassId id, class Component* component);

        /**
         * Attach a component to this entity
         * @param id
         * @param component
         */
        template <Reflect::ReflectiveClass C> requires std::derived_from<C, Component>
        void AttachComponent(std::unique_ptr<C>&& component)
        {
            AttachComponent(C::Static::Id, component.release());
        }

        /**
         * Return how many components of the given type belong to this entity
         * @param id
         * @return Component count
         */
        [[nodiscard]] int GetComponentCount(Reflect::ClassId id) const;


        /**
         * Return how many components of the given class belong to this entity
         * @return Component count
         */
        template <Reflect::ReflectiveClass C> requires std::derived_from<C, Component>
        [[nodiscard]] int GetComponentCount() const
        {
            return GetComponentCount(C::Static::Id);
        }

        /**
         * Get the name of the entity. Entity names are not guaranteed to be unique.
         * @return Entity name
         */
        [[nodiscard]] const std::string& GetName() const { return m_Name; }

        /**
         * Get the unique ID of the entity.
         * @return Entity UUID
         */
        [[nodiscard]] const TL::UUID& GetUniqueId() const { return m_UUID; }

    protected:
        friend class EntityManager;

        /**
         * Set the unique ID of the entity within an entity manager
         * @param uuid New UUID
         */
        void SetUniqueId(const TL::UUID& uuid) { m_UUID = uuid; }

        /**
         * Set the unique ID of the entity within an entity manager
         * @param uuid New UUID
         */
        void SetEntityManager(const EntityManager* entityManager) { m_EntityManager = entityManager; }
    private:
        Component* m_RootComponent;
        TL::NumericMap<Reflect::ClassId, class Component*> m_ComponentMap;

        std::string m_Name;
        TL::UUID m_UUID;
        const EntityManager* m_EntityManager;
    };

}
