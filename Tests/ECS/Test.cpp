#include <cstdio>
#include <string>
#include <Exile/Unit/Test.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

DefineComponent(PositionComponent)
{
public:
    PositionComponent()
    {

    }

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, X);
        ExposeField(Class, Y);
        ExposeField(Class, Z);
    }

private:
    int X = 0;
    int Y = 0;
    int Z = 0;
};

bool Test_ComponentConstruction()
{
    PositionComponent positionComponent;
    auto Registry = Exi::Reflect::ClassRegistry::GetInstance();
    auto Class = Registry->GetClass<PositionComponent>();
    auto Field = Class->GetField("m_Entity");

    return Field->GetType() == Exi::ECS::Entity::StaticClass::Id;
}

int main(int argc, const char** argv)
{
    const Exi::Unit::Tests tests({
        { "ComponentConstruction", Test_ComponentConstruction }
    });

    return tests.Execute(argc, argv);
}