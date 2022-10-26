#pragma once

namespace Exi::TL
{

    static constexpr inline std::uint8_t  HighNibble(std::uint8_t v) { return v >> 4; }
    static constexpr inline std::uint8_t  LowNibble(std::uint8_t v)  { return v & 0xF; }
    static constexpr inline std::uint16_t HighWord(std::uint32_t v)  { return v >> 16; }
    static constexpr inline std::uint16_t LowWord(std::uint32_t v)   { return v & 0xFFFF; }

}
