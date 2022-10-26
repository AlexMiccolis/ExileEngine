#include <cstdio>
#include <string>
#include <Exile/Unit/Test.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>

extern bool Benchmark();

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

    return Field->GetType() == Exi::ECS::Entity::Static::Id;
}

bool Test_EntityConstruction()
{
    Exi::ECS::Entity entity;
    entity.AttachComponent(std::make_unique<PositionComponent>());
    return true;
}

bool Test_EntityComponentSearch()
{
    Exi::ECS::Entity entity;
    entity.AttachComponent(std::make_unique<PositionComponent>());
    entity.AttachComponent(std::make_unique<PositionComponent>());

    std::vector<PositionComponent*> positionComponents;
    int count = entity.GetComponentsOfType<PositionComponent>(positionComponents);

    return count == 2;
}

int main(int argc, const char** argv)
{
    const Exi::Unit::Tests tests({
        { "Benchmark", Benchmark },
        { "ComponentConstruction", Test_ComponentConstruction },
        { "EntityConstruction", Test_EntityConstruction },
        { "EntityComponentSearch", Test_EntityComponentSearch }
    });

    return tests.Execute(argc, argv);
}