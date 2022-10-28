#include <Exile/Reflect/Reflection.hpp>
#include <Exile/TL/ObjectPool.hpp>
#include <Exile/Unit/Benchmark.hpp>
#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/Component.hpp>
#include <Exile/ECS/EntityManager.hpp>

DefineComponent(TransformComponent)
{
public:
    TransformComponent() = default;

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, X);
        ExposeField(Class, Y);
        ExposeField(Class, Z);
    }

private:
    double X = 0;
    double Y = 0;
    double Z = 0;
};

DeriveComponent(ExtendedTransformComponent, TransformComponent)
{

};

Exi::Unit::BenchmarkResults Benchmark_EntityGetComponentsOfType()
{
    constexpr int count = 64;
    Exi::ECS::Entity entity;
    TransformComponent* components[count] = { 0 };
    constexpr int a = sizeof(entity);

    for (int i = 0; i < count / 2; i++)
        entity.AttachComponent(std::make_unique<TransformComponent>());
    for (int i = 0; i < count / 2; i++)
        entity.AttachComponent(std::make_unique<ExtendedTransformComponent>());

    BENCHMARK_START(ComponentSearch, 65536 * 16);

    BENCHMARK_LOOP(ComponentSearch)
    {
#if 1
        int found = entity.GetComponentsOfType(TransformComponent::Static::Id,
                                               (Exi::ECS::Component**)components,
                                               count);
        if (found != count / 2)
        {
            BENCHMARK_FAIL(ComponentSearch);
            break;
        }
#else
        TransformComponent* component = entity.GetComponent<TransformComponent>();
        if (component == nullptr)
        {
            BENCHMARK_FAIL(ComponentSearch);
            break;
        }
#endif
    }

    return BENCHMARK_END(ComponentSearch);
}

DeriveClass(MySystem, Exi::ECS::System)
{
public:
    void Tick(double deltaTime) override
    {
        volatile int i = 0;
        for (auto* e : m_Entities)
        {
            if (e->GetComponent(TransformComponent::Static::Id))
                i = i + 1;
        }
    }

    bool NotifyEntity(const Exi::ECS::Entity& entity) override
    {
        if (entity.GetComponent(TransformComponent::Static::Id))
            return true;
        return false;
    }
};

Exi::Unit::BenchmarkResults Benchmark_EntityManagerAddEntity()
{
    Exi::ECS::EntityManager manager;
    MySystem system;
    manager.RegisterSystem(&system);

    auto entity1 = std::make_unique<Exi::ECS::Entity>("TransformEntity1");
    entity1->AttachComponent(std::make_unique<TransformComponent>());
    manager.AddEntity(std::move(entity1));

    auto entity2 = std::make_unique<Exi::ECS::Entity>("TransformEntity2");
    entity2->AttachComponent(std::make_unique<TransformComponent>());
    manager.AddEntity(std::move(entity2));

    BENCHMARK_START(AddEntity, 65536);
    BENCHMARK_LOOP(AddEntity)
    {
        manager.AddEntity(std::make_unique<Exi::ECS::Entity>());
        if (system.GetEntities().size() != 2)
        {
            BENCHMARK_FAIL(AddEntity);
            break;
        }
    }
    return BENCHMARK_END(AddEntity);
}

Exi::Unit::BenchmarkResults Benchmark_EntityManagerTickSystems()
{
    constexpr int count = 4096;
    Exi::ECS::EntityManager manager;
    MySystem system;
    manager.RegisterSystem(&system);

    auto entity1 = std::make_unique<Exi::ECS::Entity>("TransformEntity1");
    entity1->AttachComponent(std::make_unique<TransformComponent>());
    manager.AddEntity(std::move(entity1));

    auto entity2 = std::make_unique<Exi::ECS::Entity>("TransformEntity2");
    entity2->AttachComponent(std::make_unique<TransformComponent>());
    manager.AddEntity(std::move(entity2));

    for (int i = 0; i < 4096; i++)
        manager.AddEntity(std::make_unique<Exi::ECS::Entity>());

    BENCHMARK_START(TickSystems, 65536 * 16);
    BENCHMARK_LOOP(TickSystems)
    {
        manager.TickSystems(0);
        if (system.GetEntities().size() != 2)
        {
            BENCHMARK_FAIL(TickSystems);
            break;
        }
    }
    return BENCHMARK_END(TickSystems);
}

bool Benchmark()
{
    Exi::Unit::RunBenchmark("Entity::GetComponentsOfType", Benchmark_EntityGetComponentsOfType);
    Exi::Unit::RunBenchmark("EntityManager::AddEntity", Benchmark_EntityManagerAddEntity);
    Exi::Unit::RunBenchmark("EntityManager::TickSystems", Benchmark_EntityManagerTickSystems);
    return true;
}