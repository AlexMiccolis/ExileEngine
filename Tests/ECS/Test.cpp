#include <cstdio>
#include <string>
#include <Exile/Unit/Test.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/EntityManager.hpp>

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

bool Test_EntityManagerGetEntity()
{
    constexpr int count = 64;
    Exi::ECS::EntityManager manager;

    auto entity = std::make_unique<Exi::ECS::Entity>();
    entity->AttachComponent(std::make_unique<PositionComponent>());

    auto id = manager.AddEntity(std::move(entity));

    for (int i = 0; i < count; i++)
        manager.AddEntity(std::make_unique<Exi::ECS::Entity>());

    auto* e = manager.GetEntity(id);
    return e->GetComponentCount<PositionComponent>() == 1;
}

int main(int argc, const char** argv)
{
    const Exi::Unit::Tests tests({
        { "Benchmark", Benchmark },
        { "ComponentConstruction", Test_ComponentConstruction },
        { "EntityConstruction", Test_EntityConstruction },
        { "EntityComponentSearch", Test_EntityComponentSearch },
        { "EntityManagerGetEntity", Test_EntityManagerGetEntity }
    });

    return tests.Execute(argc, argv);
}