#pragma once

#include <Exile/Reflect/Reflection.hpp>

namespace Exi::ECS
{
    class Entity;

    DefineClass(Component)
    {
    public:
        Component();

        void AttachTo(Entity& entity);

        static void StaticInitialize(Reflect::Class& Class);
    private:
        Entity* m_Entity;
    };

}

#define DefineComponent(Name) DeriveClass(Name, Exi::ECS::Component)
#define DeriveComponent(Name, Super) DeriveClass(Name, Super)