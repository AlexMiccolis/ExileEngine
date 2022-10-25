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
        TestClass_Static = true;
        puts("TestClass::StaticInitialize()");
    }
};

DeriveClass(DerivedTestClass, TestClass)
{
public:
    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        DerivedTestClass_Static = true;
        puts("DerivedTestClass::StaticInitialize()");
    }
};

bool Test_StaticInitialize()
{
    DerivedTestClass test;
    return TestClass_Static && DerivedTestClass_Static;
}

using TestFn = bool(*)();
static const std::unordered_map<std::string, TestFn> s_TestFunctions {
    { "Benchmark", Benchmark },
    { "StaticInitialize", Test_StaticInitialize }
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