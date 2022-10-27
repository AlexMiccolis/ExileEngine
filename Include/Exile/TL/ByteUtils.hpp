#pragma once

#include <bit>

namespace Exi::TL
{

    static constexpr inline std::uint8_t  HighNibble(std::uint8_t v) { return v >> 4; }
    static constexpr inline std::uint8_t  LowNibble(std::uint8_t v)  { return v & 0xF; }
    static constexpr inline std::uint16_t HighWord(std::uint32_t v)  { return v >> 16; }
    static constexpr inline std::uint16_t LowWord(std::uint32_t v)   { return v & 0xFFFF; }

    static inline std::int16_t NetShort(std::int16_t n)
    {
        if constexpr (std::endian::native == std::endian::big)
            return n;
        return static_cast<std::int16_t>(__builtin_bswap16(n));
    }

    static inline std::int32_t NetWord(std::int32_t n)
    {
        if constexpr (std::endian::native == std::endian::big)
            return n;
        return static_cast<std::int32_t>(__builtin_bswap32(n));
    }

    static inline std::int64_t NetLong(std::int64_t n)
    {
        if constexpr (std::endian::native == std::endian::big)
            return n;
        return static_cast<std::int64_t>(__builtin_bswap64(n));
    }

    static inline std::uint16_t NetShort(std::uint16_t n) { return NetShort(static_cast<std::int16_t>(n)); }
    static inline std::uint32_t NetWord(std::uint32_t n)  { return NetWord(static_cast<std::int32_t>(n)); }
    static inline std::uint64_t NetLong(std::uint64_t n)  { return NetLong(static_cast<std::int64_t>(n)); }
}
