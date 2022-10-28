#pragma once

#include <bit>

#ifdef _MSC_VER
    #include <cstdlib>
    #include <intrin.h>
    #define BYTESWAP16(x) _byteswap_ushort(x)
    #define BYTESWAP32(x) _byteswap_ulong(x)
    #define BYTESWAP64(x) _byteswap_uint64(x)
#else
    #define BYTESWAP16(x) __builtin_bswap16(x)
    #define BYTESWAP32(x) __builtin_bswap32(x)
    #define BYTESWAP64(x) __builtin_bswap64(x)
    #ifdef __x86_64__
        #define POPCNT(x)   __builtin_popcountll(x)
        #define FFS(x)      __builtin_ffsll(x)
    #else
        #define POPCNT(x)   __builtin_popcount(x)
        #define FFS(x)      __builtin_ffs(x)
    #endif
#endif

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
        return static_cast<std::int16_t>(BYTESWAP16(n));
    }

    static inline std::int32_t NetWord(std::int32_t n)
    {
        if constexpr (std::endian::native == std::endian::big)
            return n;
        return static_cast<std::int32_t>(BYTESWAP32(n));
    }

    static inline std::int64_t NetLong(std::int64_t n)
    {
        if constexpr (std::endian::native == std::endian::big)
            return n;
        return static_cast<std::int64_t>(BYTESWAP64(n));
    }

    static inline std::uint16_t NetShort(std::uint16_t n) { return NetShort(static_cast<std::int16_t>(n)); }
    static inline std::uint32_t NetWord(std::uint32_t n)  { return NetWord(static_cast<std::int32_t>(n)); }
    static inline std::uint64_t NetLong(std::uint64_t n)  { return NetLong(static_cast<std::int64_t>(n)); }

    /**
     * Count number of set bits in value `i`
     * @param i
     * @return Bits
     */
    static constexpr inline int PopCount(std::size_t i)
    {
        return POPCNT(i);
    }

    /**
     * Get the zero-indexed position of the first set bit in value `i`
     * @param i
     * @return Position of first set bit, -1 if no bits are set
     */
    static constexpr inline int FindFirstSet(std::size_t i)
    {
        if (!i) return -1;
        return FFS(i) - 1;
    }

}
