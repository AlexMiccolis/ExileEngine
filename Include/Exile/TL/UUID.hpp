#pragma once

#include <cstdint>
#include <random>
#include <string>

namespace Exi::TL
{

    /**
     * UUID structured like {AAAAAAAA-BBBB-CCCC-DDDD-DDDDDDDDDDDD}
     */
    struct UUID
    {
        union
        {
            struct
            {
                uint32_t a;
                uint16_t b;
                uint16_t c;
                uint64_t d;
            };
            std::array<uint8_t, 16> bytes;
        };

        UUID(uint32_t _a = 0, uint16_t _b = 0, uint16_t _c = 0, uint64_t _d = 0)
            : a(_a), b(_b), c(_c), d(_d) { }
        UUID(const std::array<uint8_t, 16>& _bytes)
            : bytes(_bytes) { }
        UUID(const UUID& uuid)
            : bytes(uuid.bytes) { }

        bool operator==(const UUID& uuid) const
        {
            return (a == uuid.a) && (b == uuid.b) && (c == uuid.c) && (d == uuid.d);
        }

        bool operator!=(const UUID& uuid) const { return !(*this == uuid); }

        void ToString(char buffer[39]) const
        {
            snprintf(buffer, 39, "{%08X-%04X-%04X-%04llX-%012llX}", a, b, c, d & 0xFFFFUL, d >> 16UL);
        }

        std::string ToString() const
        {
            std::string s;
            s.resize(39);
            ToString(s.data());
            return s;
        }

        static UUID Random()
        {
            static std::independent_bits_engine<std::default_random_engine, 8, uint8_t> s_Generator;
            static bool s_Seeded = false;

            if (!s_Seeded)
            {
                s_Generator.seed(__builtin_ia32_rdtsc());
                s_Seeded = true;
            }

            std::array<uint8_t, 16> bytes = { };
            std::generate(bytes.begin(), bytes.end(), std::ref(s_Generator));
            return UUID(bytes);
        }

    };

}

template <> struct std::hash<Exi::TL::UUID>
{
    std::size_t operator()(const Exi::TL::UUID& k) const
    {
        std::size_t h = ~0;

        for (int i = 0; i < 16; i++)
        {
            if constexpr(sizeof(std::size_t) == 8)
            h <<= 4;
            else
                h <<= 2;
            h ^= std::hash<uint8_t>()(k.bytes[i]);
        }
        return h;
    }
};
