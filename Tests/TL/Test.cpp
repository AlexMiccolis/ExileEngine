#include <Exile/Unit/Test.hpp>
#include <Exile/TL/NumericMap.hpp>

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

    if (count == 0)
        return false;

    return values[0] == -1;
}

bool Test_NumericMap_GetKeys()
{
    Exi::TL::NumericMap<std::size_t, int> map;

    map.Emplace(1, 1);
    map.Emplace(2, 4);
    map.Emplace(3, 5);
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

int main(int argc, const char** argv)
{
    Exi::Unit::Tests tests ({
        { "NumericMap_Find", Test_NumericMap_Find },
        { "NumericMap_GetKeys", Test_NumericMap_GetKeys },
        { "Benchmark", Benchmark }
    });
    return tests.Execute(argc, argv);
}
