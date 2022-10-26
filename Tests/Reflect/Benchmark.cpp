#include <Exile/Reflect/Reflection.hpp>
#include <Exile/TL/ObjectPool.hpp>
#include <Exile/Unit/Benchmark.hpp>

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
        : m_A(1), m_B(2) { }

    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, m_A);
        ExposeField(Class, m_B);
        ExposeMethod(Class, FieldClass, SetA);
        ExposeMethod(Class, FieldClass, SetAB);
        ExposeMethod(Class, FieldClass, GetA);
    }

private:
    void SetA(int a1, int a2)
    {
        m_A = a1 * a2;
    }

    void SetAB(float a, float b)
    {
        m_A = a;
        m_B = b;
    }

    int GetA() const { return m_A; }

    int m_A;
    int m_B;
};

class NonReflectivePODClass
{
public:
    NonReflectivePODClass()
            : a(1) { }
    int a;
};

Exi::Unit::BenchmarkResults Benchmark_NonReflectiveConstruction()
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

Exi::Unit::BenchmarkResults Benchmark_Construction()
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

Exi::Unit::BenchmarkResults Benchmark_DerivedConstruction()
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

Exi::Unit::BenchmarkResults Benchmark_FieldGet()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("m_A");
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

Exi::Unit::BenchmarkResults Benchmark_NaiveFieldGet()
{
    BENCHMARK_START(NaiveFieldGet, 65536 * 16);

    BENCHMARK_LOOP(NaiveFieldGet)
    {
        FieldClass cls;
        auto instance = Exi::Reflect::ClassRegistry::GetInstance();
        auto fieldClass = instance->GetClass<FieldClass>();
        auto field = fieldClass->GetField("m_A");
        auto value = field->Get(&cls);
        if (value.GetType() != Exi::Reflect::TypeInt32 || value.Get<int>() != 1)
        {
            BENCHMARK_FAIL(NaiveFieldGet);
            break;
        }
    }

    return BENCHMARK_END(NaiveFieldGet);
}

Exi::Unit::BenchmarkResults Benchmark_FieldSet()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("m_A");
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

Exi::Unit::BenchmarkResults Benchmark_MethodInvokeUnchecked()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("m_A");
    auto method = fieldClass->GetMethod("SetA");
    FieldClass cls;

    BENCHMARK_START(MethodInvokeUnchecked, 65536 * 16);

    BENCHMARK_LOOP(MethodInvokeUnchecked)
    {
        void* p = method->InvokeUnchecked(&cls, (int)Iteration, (int)1);
        if (field->GetValue<int>(&cls) != Iteration)
        {
            BENCHMARK_FAIL(MethodInvokeUnchecked);
            break;
        }
    }

    return BENCHMARK_END(MethodInvokeUnchecked);
}

Exi::Unit::BenchmarkResults Benchmark_MethodInvoke()
{
    auto instance = Exi::Reflect::ClassRegistry::GetInstance();
    auto fieldClass = instance->GetClass<FieldClass>();
    auto field = fieldClass->GetField("m_A");
    auto method = fieldClass->GetMethod("SetA");
    FieldClass cls;

    BENCHMARK_START(MethodInvoke, 65536 * 16);

    BENCHMARK_LOOP(MethodInvoke)
    {
        Exi::Reflect::TypedValue Parameters[] {
            { Exi::Reflect::TypeInt32, Iteration },
            { Exi::Reflect::TypeInt32, 1 }
        };
        Exi::Reflect::TypedValue RetVal = method->Invoke(&cls, Parameters, 2);
        if (field->GetValue<int>(&cls) != Iteration)
        {
            BENCHMARK_FAIL(MethodInvoke);
            break;
        }
    }

    return BENCHMARK_END(MethodInvoke);
}

bool RunBenchmark(const char* Name, Exi::Unit::BenchmarkResults(*Fn)())
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
    RunBenchmark("Unchecked Method Invoke", Benchmark_MethodInvokeUnchecked);
    RunBenchmark("Method Invoke", Benchmark_MethodInvoke);

    return true;
}