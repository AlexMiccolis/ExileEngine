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
    template <int N, typename Elem = std::size_t>
    class FreeMap
    {
    public:
        using Index   = std::size_t;
        using Element = Elem;
        static constexpr Element InvalidIndex = static_cast<Element>(SIZE_MAX);
        static constexpr int ElementBytes     = sizeof(Element);
        static constexpr int ElementBits      = ElementBytes * 8;
        static constexpr int Elements         = RoundUp(N, ElementBits) / ElementBits;
        static constexpr int Bits = N;

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
                Index idx = (i * ElementBits) + bit;

                if (bit < 0)
                    continue;

                if (idx >= Bits)
                    break;

                m_Elements[i] &= ~(1ULL << bit);
                return idx;
            }
            return InvalidIndex;
        }

        /**
         * Free an index
         * @param i
         */
        void Free(Index i)
        {
            if (i == InvalidIndex)
                return;
            Element& e = m_Elements.at(i / ElementBits);
            int bit = i % ElementBits;
            e |= (1ULL << bit);
        }

        /**
         * Check if a given index is free
         * @param i
         * @return True or false
         */
        [[nodiscard]] bool IsFree(Index i) const
        {
            if (i == InvalidIndex)
                return false;
            Element e = m_Elements.at(i / ElementBits);
            int bit = i % ElementBits;
            return e & (1ULL << bit);
        }

        [[nodiscard]] bool IsEmpty() const
        {
            for (int i = 0; i < Elements; i++)
            {
                Element e = m_Elements[i];
                if (e != InvalidIndex)
                    return false;
            }
            return true;
        }

        [[nodiscard]] bool IsFull() const
        {
            for (int i = 0; i < Elements; i++)
            {
                Element e = m_Elements[i];
                if (e != 0)
                    return false;
            }
            return true;
        }
    private:
        std::array<Element, Elements> m_Elements;
    };

}
