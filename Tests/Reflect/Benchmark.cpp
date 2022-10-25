#include <Exile/Reflect/Reflection.hpp>
#include <Exile/TL/ObjectPool.hpp>
#include "Benchmark.hpp"

DefineClass(PODClass)
{
public:
    PODClass()
        : a(1)
    {

    }

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {

    }

    int a;
};

DeriveClass(DerivedPODClass, PODClass)
{
public:
    DerivedPODClass()
        : b(2)
    {

    }

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {

    }

    int b;
};

DefineClass(FieldClass)
{
public:
    FieldClass()
        : a(1), b(2) { }

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, FieldClass, a);
        ExposeField(Class, FieldClass, b);
    }
private:
    int a;
    int b;
};

class NonReflectivePODClass
{
public:
    NonReflectivePODClass()
            : a(1) { }
    int a;
};

BenchmarkResults Benchmark_NonReflectiveConstruction()
{
    Exi::TL::ObjectPool<NonReflectivePODClass> objectPool;
    BENCHMARK_START(NonReflectiveConstruction, 65536 * 16);

    BENCHMARK_LOOP(NonReflectiveConstruction)
    {
        auto* pod = objectPool.Get();
        if (pod->a != 1)
        {
            BENCHMARK_FAIL(NonReflectiveConstruction);
            break;
        }
        objectPool.Release(pod);
    }

    return BENCHMARK_END(NonReflectiveConstruction);
}

BenchmarkResults Benchmark_Construction()
{
    Exi::TL::ObjectPool<PODClass> objectPool;
    BENCHMARK_START(ObjectConstruction, 65536 * 16);

    BENCHMARK_LOOP(ObjectConstruction)
    {
        auto* pod = objectPool.Get();
        if (pod->a != 1)
        {
            BENCHMARK_FAIL(ObjectConstruction);
            break;
        }
        objectPool.Release(pod);
    }

    return BENCHMARK_END(ObjectConstruction);
}

BenchmarkResults Benchmark_DerivedConstruction()
{
    Exi::TL::ObjectPool<DerivedPODClass> objectPool;
    BENCHMARK_START(DerivedObjectConstruction, 65536 * 16);

    BENCHMARK_LOOP(DerivedObjectConstruction)
    {
        auto* pod = objectPool.Get();
        if (pod->a != 1 || pod->b != 2)
        {
            BENCHMARK_FAIL(DerivedObjectConstruction);
            break;
        }
        objectPool.Release(pod);
    }

    return BENCHMARK_END(DerivedObjectConstruction);
}

BenchmarkResults Benchmark_FieldGet()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("a");
    FieldClass cls;

    BENCHMARK_START(FieldGet, 65536 * 16);

    BENCHMARK_LOOP(FieldGet)
    {
        auto value = field->Get(&cls);
        if (value.GetType() != Exi::Reflect::TypeInt32 || value.Get<int>() != 1)
        {
            BENCHMARK_FAIL(FieldGet);
            break;
        }
    }

    return BENCHMARK_END(FieldGet);
}

BenchmarkResults Benchmark_NaiveFieldGet()
{
    BENCHMARK_START(NaiveFieldGet, 65536 * 16);

    BENCHMARK_LOOP(NaiveFieldGet)
    {
        FieldClass cls;
        auto instance = Exi::Reflect::ClassRegistry::GetInstance();
        auto fieldClass = instance->GetClass<FieldClass>();
        auto field = fieldClass->GetField("a");
        auto value = field->Get(&cls);
        if (value.GetType() != Exi::Reflect::TypeInt32 || value.Get<int>() != 1)
        {
            BENCHMARK_FAIL(NaiveFieldGet);
            break;
        }
    }

    return BENCHMARK_END(NaiveFieldGet);
}

BenchmarkResults Benchmark_FieldSet()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("a");
    FieldClass cls;

    BENCHMARK_START(FieldGet, 65536 * 16);

    BENCHMARK_LOOP(FieldGet)
    {
        field->SetValue<int>(&cls, Iteration);
        if (field->GetValue<int>(&cls) != Iteration)
            break;
    }

    return BENCHMARK_END(FieldGet);
}

bool RunBenchmark(const char* Name, BenchmarkResults(*Fn)())
{
    auto Results = Fn();
    if (Results.Failed)
    {
        printf("%s: FAILED\n", Name);
        return false;
    }

    printf("%s: %llu iterations, %llu ns/iteration, %.07f total seconds\n",
           Name,
           Results.IterationsCount,
           Results.NanosPerIteration,
           Results.TotalSeconds);
    return true;
}

bool Benchmark()
{
    RunBenchmark("Non-Reflective Object Construction", Benchmark_NonReflectiveConstruction);
    RunBenchmark("Reflective Object Construction", Benchmark_DerivedConstruction);
    RunBenchmark("Field Get", Benchmark_FieldGet);
    RunBenchmark("Field Set", Benchmark_FieldSet);
    RunBenchmark("Naive Field Get", Benchmark_NaiveFieldGet);

    FieldClass Instance;
    auto Class = Exi::Reflect::ClassRegistry::GetInstance()->GetClass<FieldClass>();
    auto Field = Class->GetField("a");

    auto BeforeValue = Field->Get(&Instance);
    Exi::Reflect::TypedValue SetVal(Exi::Reflect::TypeInt32, 100);
    Field->Set(&Instance, SetVal);
    auto AfterValue = Field->Get(&Instance);

    printf("%d %d\n", BeforeValue.Get<int>(), AfterValue.Get<int>());
    return true;
}