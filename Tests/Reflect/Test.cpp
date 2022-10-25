#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include <Exile/Reflect/Reflection.hpp>

extern bool Benchmark();

static bool TestClass_Static = false;
static bool DerivedTestClass_Static = false;

DefineClass(TestClass)
{
public:
    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, TestClass, m_MyInt);
        TestClass_Static = true;
        puts("TestClass::StaticInitialize()");
    }

    int m_MyInt = 1;
};

DeriveClass(DerivedTestClass, TestClass)
{
public:
    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        ExposeField(Class, DerivedTestClass, m_MyOtherInt);
        DerivedTestClass_Static = true;
        puts("DerivedTestClass::StaticInitialize()");
    }

    int m_MyOtherInt = 2;
};

bool Test_StaticInitialize()
{
    DerivedTestClass test;
    return TestClass_Static && DerivedTestClass_Static;
}

bool Test_FieldGet()
{
    DerivedTestClass Instance;
    auto* Registry = Exi::Reflect::ClassRegistry::GetInstance();
    auto* Class = Registry->GetClass<DerivedTestClass>();
    auto* IntField = Class->GetField("m_MyInt");
    auto* OtherIntField = Class->GetField("m_MyOtherInt");

    if (IntField == nullptr || OtherIntField == nullptr)
        return false;

    Exi::Reflect::TypedValue myInt = IntField->Get(&Instance);
    Exi::Reflect::TypedValue myOtherInt = OtherIntField->Get(&Instance);
    if (myInt.Get<int>() != 1 || myOtherInt.Get<int>() != 2)
        return false;

    return true;
}

bool Test_FieldSet()
{
    DerivedTestClass Instance;
    auto* Registry = Exi::Reflect::ClassRegistry::GetInstance();
    auto* Class = Registry->GetClass<DerivedTestClass>();
    auto* IntField = Class->GetField("m_MyInt");
    auto* OtherIntField = Class->GetField("m_MyOtherInt");

    if (IntField == nullptr || OtherIntField == nullptr)
        return false;

    Exi::Reflect::TypedValue myInt = IntField->Get(&Instance);
    Exi::Reflect::TypedValue myOtherInt = OtherIntField->Get(&Instance);
    if (myInt.Get<int>() != 1 || myOtherInt.Get<int>() != 2)
        return false;

    myInt.Set((int)0x5555);
    myOtherInt.Set((int)0xAAAA);

    IntField->Set(&Instance, myInt);
    OtherIntField->Set(&Instance, myOtherInt);

    myInt = IntField->Get(&Instance);
    myOtherInt = OtherIntField->Get(&Instance);
    if (myInt.Get<int>() != 0x5555 || myOtherInt.Get<int>() != 0xAAAA)
        return false;

    return true;
}

using TestFn = bool(*)();
static const std::unordered_map<std::string, TestFn> s_TestFunctions {
    { "Benchmark", Benchmark },
    { "StaticInitialize", Test_StaticInitialize },
    { "FieldGet", Test_FieldGet },
    { "FieldSet", Test_FieldSet }
};

int main(int argc, const char** argv)
{
    std::vector<std::string> args(&argv[0], &argv[argc]);

    if (argc < 2)
    {
        puts("ERROR: Test name not specified, available tests:");
        for (const auto& pair : s_TestFunctions)
            printf("    %s\n", pair.first.c_str());
        return 1;
    }

    const std::string& testName = args[1];
    if (s_TestFunctions.contains(testName))
    {
        TestFn fn = s_TestFunctions.at(testName);
        return fn ? !fn() : 1;
    }
    printf("ERROR: Unknown test name '%s'\n", testName.c_str());
    return 1;
}