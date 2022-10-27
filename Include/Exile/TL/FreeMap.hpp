#pragma once

#include <array>
#include <cstddef>
#include <Exile/TL/Range.hpp>
#include <Exile/TL/ByteUtils.hpp>

namespace Exi::TL
{

    /**
     * Fixed-size bitset representing a region of free and used objects
     * @tparam N Number of bits
     */
    template <int N>
    class FreeMap
    {
    public:
        using Index   = std::size_t;
        using Element = std::size_t;
        static constexpr Index InvalidIndex = SIZE_MAX;
        static constexpr int ElementBytes   = sizeof(Element);
        static constexpr int ElementBits    = ElementBytes * 8;
        static constexpr int Elements       = RoundUp(N, ElementBits) / ElementBits;

        static_assert(Elements >= 1, "Zero-bit FreeMap is invalid");

        FreeMap() { std::fill_n(m_Elements.begin(), Elements, InvalidIndex); }

        /**
         * Allocate an index
         * @return Index if found, InvalidIndex otherwise
         */
        Index Allocate()
        {
            for (int i = 0; i < Elements; i++)
            {
                Element e = m_Elements[i];
                int bit   = FindFirstSet(e);

                if (bit < 0)
                    continue;

                m_Elements[i] &= ~(1ULL << bit);
                return (i * ElementBits) + bit;
            }
            return InvalidIndex;
        }

        /**
         * Free an index
         * @param i
         */
        void Free(Index i)
        {
            Element& e = m_Elements[i / ElementBits];
            int bit = i % ElementBits;
            e |= (1ULL << bit);
        }
    private:
        std::array<Element, Elements> m_Elements;
    };

}
