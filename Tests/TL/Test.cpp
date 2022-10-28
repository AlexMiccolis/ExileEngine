#include <Exile/Unit/Test.hpp>
#include <Exile/TL/NumericMap.hpp>
#include <Exile/TL/FreeMap.hpp>

extern bool Benchmark();

bool Test_NumericMap_Find()
{
    Exi::TL::NumericMap<std::size_t, int> map;

    map.Emplace(1, 1);
    map.Emplace(2, 4);
    map.Emplace(3, 5);
    map.Emplace(0, -1);

    int values[1];
    int count = map.Find(0, values, 1);

    if (count != 1)
        return false;

    return values[0] == -1;
}

bool Test_NumericMap_GetKeys()
{
    Exi::TL::NumericMap<std::size_t, int> map;

    map.Emplace(1, 1);
    map.Emplace(1, 3);
    map.Emplace(2, 4);
    map.Emplace(3, 5);
    map.Emplace(3, 2);
    map.Emplace(0, -1);

    std::size_t keys[64];
    int count = map.GetKeys(keys, 64);

    if (count == 0)
        return false;

    for (int i = 0; i < count; i++)
    {
        if (keys[i] == 3)
            return true;
    }

    return false;
}

bool Test_NumericMap_Contract()
{
    Exi::TL::NumericMap<std::size_t, int> map;
    map.Expand();
    map.Emplace(1, 1);
    int freed = map.Contract();
    return freed == (map.Rows * (map.Columns - 1));
}

bool Test_FreeMap_Allocate()
{
    Exi::TL::FreeMap<1024> map;
    constexpr int count = 66;
    std::size_t index;

    for (int i = 0; i < count; i++)
        index = map.Allocate();
    map.Free(index);
    return index == (count - 1);
}

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "NumericMap_Find", Test_NumericMap_Find },
        { "NumericMap_GetKeys", Test_NumericMap_GetKeys },
        { "NumericMap_Contract", Test_NumericMap_Contract },
        { "FreeMap_Allocate", Test_FreeMap_Allocate },
        { "Benchmark", Benchmark }
    });

    return tests.Execute(argc, argv);
}
