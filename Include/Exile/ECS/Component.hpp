#pragma once

#include <Exile/Reflect/Reflection.hpp>

namespace Exi::ECS
{

    DefineClass(Component)
    {
    public:
        Component();
        
        static void StaticInitialize(Reflect::Class& Class);
    private:
    };

}