#pragma once

#include <functional>
#include <iterator>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <array>

#include <Exile/TL/ByteUtils.hpp>
#include <Exile/TL/FreeMap.hpp>

namespace Exi::TL
{
    static constexpr inline std::uint32_t KeyReduce32(std::size_t key)
    {
        if constexpr (sizeof(std::size_t) == sizeof(std::uint32_t))
            return static_cast<uint32_t>(key);
        else
        {
            uint32_t high = (key >> 32ULL);
            uint32_t low  = (key & 0xFFFFFFFFULL);
            return high ^ low;
        }
    }

    static constexpr inline std::uint8_t KeySplit4(std::uint8_t key)
    {
        uint8_t a = (key & 0xF0) >> 4;
        uint8_t b = key & 0x0F;

        uint8_t x = a ^ b;
        uint8_t y = a & b;

        return (y << 4) | x;
    }

    static constexpr inline std::uint32_t KeySplit16(std::uint32_t key)
    {
        uint8_t a = KeySplit4(key & 0xFF);
        uint8_t b = KeySplit4((key >> 8) & 0xFF);
        uint8_t c = KeySplit4((key >> 16) & 0xFF);
        uint8_t d = KeySplit4((key >> 24) & 0xFF);

        uint16_t x = (LowNibble(d) << 12)
                     | (LowNibble(c) << 8)
                     | (LowNibble(b) << 4)
                     | LowNibble(a);
        uint16_t y = (HighNibble(d) << 12)
                     | (HighNibble(c) << 8)
                     | (HighNibble(b) << 4)
                     | HighNibble(a);

        return ((y / 0xFFF) << 16) | (x / 0xFFF);
    }

    /**
     * Numeric map, a multimap with a numeric key.
     * Works best with keys that have high entropy.
     * @tparam K
     * @tparam V
     * @tparam B
     */
    template <std::integral K, class V, int X = 32 / sizeof(void*), int Y = 32 / sizeof(void*)>
    class NumericMap
    {
    public:
        using Key   = K;
        using Value = V;
        static constexpr int Rows    = X;
        static constexpr int Columns = Y;
        static constexpr int Buckets = Rows * Columns;

        static constexpr int KeysPerHead = 4;
        static constexpr int ValuesPerHead = 8;

        struct ValueNode
        {
            Key key;
            Value value;
            ValueNode* next;

            ValueNode() = default;
            ValueNode(Key _key) : key(_key), next(nullptr) { }
        };

        struct KeyNode
        {
            Key           key;
            std::uint32_t count;
            ValueNode*    first;
        };

        struct alignas(64) RowHead
        {
            KeyNode keys[KeysPerHead];
            ValueNode values[ValuesPerHead];
            FreeMap<KeysPerHead, uint16_t> freeKeys;
            FreeMap<ValuesPerHead, uint16_t> freeValues;
            RowHead* next;

            RowHead() : next(nullptr) { }
        };

        using RowType    = std::array<RowHead, Rows>;
        using BucketPos  = uint32_t;

        NumericMap() : m_Keys(0), m_BucketColumns { nullptr } { }
        ~NumericMap()
        {
            for (int c = 0; c < Columns; c++)
            {
                auto* col = m_BucketColumns[c];

                if (col == nullptr)
                    continue;

                for (int r = 0; r < Rows; r++)
                {
                    auto* row = col->at(r).next;
                    while (row != nullptr)
                    {
                        auto* lastRow = row;
                        row = row->next;
                        delete lastRow;
                    }
                }

                delete col;
            }
        }

        static constexpr inline BucketPos GetBucket(Key key)
        {
            return KeySplit16(KeyReduce32(key));
        }

        /**
         * Emplace a key and value into the map
         * @param key
         * @param value
         * @return Reference to emplaced value
         */
        Value& Emplace(Key key, Value value)
        {
            ValueNode* node = InsertNode(key);
            node->value = value;
            return node->value;
        }

        /**
         * Check if any values in the map match the given key
         * @param key
         * @return
         */
        bool Contains(Key key) const
        {
            return FindKey(key) != nullptr;
        }

        /**
         * Count the number of values matching the given key
         * @param key
         * @return
         */
        std::size_t Count(Key key) const
        {
            auto keyNode = FindKey(key);
            if (!keyNode)
                return 0;
            return keyNode->count;
        }

        /**
         * Find all values matching a key and copy them into an array
         * @param key
         * @param values
         * @param maxValues
         * @return Number of values found
         */
        std::size_t Find(Key key, Value* values, std::size_t maxValues) const
        {
            const KeyNode* keyNode = FindKey(key);
            std::size_t count;

            if (!keyNode)
                return 0;

            ValueNode* node = keyNode->first;
            for (count = 0; count < maxValues && count < keyNode->count; count++)
            {
                values[count] = node->value;
                node = node->next;
            }

            return keyNode->count;
        }

        /**
         * Retrieve all keys in the map and copy them into an array.
         * Will write no more than `maxKeys` keys into `keys`.
         * @param keys Pointer to array of keys
         * @param maxKeys Size of key array
         * @return Number of keys present in map
         */
        std::size_t GetKeys(Key* keys, std::size_t maxKeys) const
        {
            std::size_t count = 0;
            for (int c = 0; c < Columns; c++)
            {
                auto* colPtr = m_BucketColumns[c];

                if (!colPtr)
                    continue;

                for (int r = 0; r < Rows; r++)
                {
                    RowHead* row = &(*colPtr)[r];
                    GetRowKeys(row, keys, maxKeys, count);
                }
            }
            return count;
        }

        /**
         * Get the number of keys in the map
         * @return Key count
         */
        [[nodiscard]] std::size_t GetKeys() const { return m_Keys; }

        /**
         * Expand the map to it's maximum capacity.
         * This can speed up insertions at the cost of memory.
         */
        void Expand()
        {
            for (int i = 0; i < Columns; i++)
            {
                if (!m_BucketColumns[i])
                    m_BucketColumns[i] = new RowType();
            }
        }

        /**
         * Search the map for empty bucket rows and prune them to save memory.
         * @return Number of buckets freed
         */
        int Contract()
        {
            int freed = 0;
            for (int i = 0; i < Columns; i++)
            {
                RowType* row = m_BucketColumns[i];
                bool empty = true;

                if (!row)
                    continue;

                for (int r = 0; r < Rows; r++)
                {
                    if (!row->at(r).freeValues.IsEmpty())
                    {
                        empty = false;
                        break;
                    }
                }

                if (empty)
                {
                    m_BucketColumns[i] = nullptr;
                    delete row;
                    freed += Rows;
                }
            }
            return freed;
        }

    private:
        void GetRowKeys(RowHead* row, Key* keys, std::size_t maxKeys, std::size_t& count) const
        {
            do
            {
                for (int k = 0; k < KeysPerHead; k++)
                {
                    if (row->freeKeys.IsFree(k))
                        continue;

                    if (count < maxKeys)
                        keys[count] = row->keys[k].key;
                    ++count;
                }
                row = row->next;
            } while (row != nullptr);
        }

        ValueNode* InsertNode(Key key)
        {
            BucketPos pos = GetBucket(key);
            RowHead&  row = GetRow(pos)->at(LowWord(pos) % Rows);
            ValueNode* valueNode;
            KeyNode* keyNode;

            // Allocate key if key list is empty or if we cant find our node
            if (row.freeKeys.IsEmpty() || !(keyNode = FindKey(row, key)))
            {
                keyNode = AllocateKey(row);
                keyNode->key = key;
                keyNode->count = 0;
                m_Keys++;
            }

            valueNode = AllocateValue(row);
            valueNode->key = key;

            valueNode->next = keyNode->first;
            keyNode->first = valueNode;
            keyNode->count++;
            return valueNode;
        }

        KeyNode* FindKey(RowHead& row, Key key) const
        {
            for (int i = 0; i < KeysPerHead; i++)
            {
                if (row.keys[i].key == key)
                    if (!row.freeKeys.IsFree(i))
                        return &row.keys[i];
            }

            if (row.next == nullptr)
                return nullptr;
            return FindKey(*row.next, key);
        }

        const KeyNode* FindKey(Key key) const
        {
            auto  pos = GetBucket(key);
            auto* row = m_BucketColumns[HighWord(pos) % Columns];
            auto& rowHead = row->at(LowWord(pos) % Rows);
            return FindKey(rowHead, key);
        }

        KeyNode* AllocateKey(RowHead& row)
        {
            int idx = row.freeKeys.Allocate();
            if (idx != row.freeKeys.InvalidIndex)
            {
                KeyNode* node = &row.keys[idx];
                return node;
            }
            if (!row.next)
                row.next = new RowHead();
            return AllocateKey(*row.next);
        }

        ValueNode* AllocateValue(RowHead& row)
        {
            int idx = row.freeValues.Allocate();
            if (idx != row.freeValues.InvalidIndex)
            {
                ValueNode* node = &row.values[idx];
                return node;
            }
            if (!row.next)
                row.next = new RowHead();
            return AllocateValue(*row.next);
        }

        /**
         * Get the row pointer for a given bucket position
         * @param pos
         * @return
         */
        RowType* GetRow(BucketPos pos)
        {
            RowType* row = m_BucketColumns[HighWord(pos) % Columns];
            if (!row)
            {
                row = m_BucketColumns[HighWord(pos) % Columns] = new RowType { };
            }
            return row;
        }

        std::size_t m_Keys;
        std::array<RowType*, Columns> m_BucketColumns;
    };

}
