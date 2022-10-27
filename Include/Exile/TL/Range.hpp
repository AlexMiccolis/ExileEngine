#pragma once

namespace Exi::TL
{

    template <typename T>
    static constexpr inline T RoundUp(T round, T to)
    {
        return round + ((round % to == 0) ? 0 : to - (round % to));
    }

}
