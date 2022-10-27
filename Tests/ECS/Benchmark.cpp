#include <Exile/Reflect/Reflection.hpp>
#include <Exile/TL/ObjectPool.hpp>
#include <Exile/Unit/Benchmark.hpp>
#include <Exile/ECS/Entity.hpp>
#include <Exile/ECS/Component.hpp>

DefineComponent(TransformComponent)
{
public:
    TransformComponent()
    {

    }

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

Exi::Unit::BenchmarkResults Benchmark_ComponentSearch()
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

bool Benchmark()
{
    Exi::Unit::RunBenchmark("Entity Component Search", Benchmark_ComponentSearch);
    return true;
}