#pragma once

#include <functional>
#include <iterator>
#include <concepts>
#include <cstdint>
#include <cstddef>
#include <array>

#include <Exile/TL/ByteUtils.hpp>

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

        struct BucketNode
        {
            Key key;
            Value value;
            BucketNode* next;

            BucketNode() = default;
            BucketNode(Key _key) : key(_key), next(nullptr) { }
        };

        struct KeyNode
        {
            Key         key;
            BucketNode* first;
            KeyNode*    next;
            std::size_t count;
        };

        struct RowHead
        {
            BucketNode* bucketNode = nullptr;
            KeyNode*    keyNode    = nullptr;
        };

        using RowType    = std::array<RowHead, Rows>;
        using BucketPos  = uint32_t;

        static constexpr int MaxMemory = (sizeof(RowType*) * Columns) + ((sizeof(RowHead) * Rows) * Columns);

        NumericMap() : m_BucketColumns { nullptr } { }
        ~NumericMap()
        {
            for (int c = 0; c < Columns; c++)
            {
                auto* col = m_BucketColumns[c];

                if (col == nullptr)
                    continue;

                for (int r = 0; r < Rows; r++)
                {
                    RowHead& row = (*col)[r];
                    DestroyBucketList(row);
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
            BucketNode* node = InsertEmpty(key);
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
            return FindFirst(key) != nullptr;
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
            auto keyNode = FindKey(key);
            std::size_t i;
            BucketNode* node;

            if (!keyNode)
                return 0;

            node = keyNode->first;
            for (i = 0; i < maxValues && i < keyNode->count; i++)
            {
                values[i] = node->value;
                node = node->next;
            }

            return i;
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
                    RowHead& row = (*colPtr)[r];
                    KeyNode* keyNode = row.keyNode;
                    while (keyNode != nullptr)
                    {
                        if (count < maxKeys)
                            keys[count] = keyNode->key;
                        keyNode = keyNode->next;
                        ++count;
                    }
                }
            }
            return count;
        }

        /**
         * Get the number of keys in the map
         * @return Key count
         */
        [[nodiscard]] std::size_t GetKeys() const { return GetKeys(nullptr, 0); }

        /**
         * Expand the map to it's maximum capacity.
         * This can speed up insertions at the cost of memory.
         */
        void Expand()
        {
            for (int i = 0; i < Columns; i++)
            {
                if (!m_BucketColumns[i])
                    m_BucketColumns[i] = new RowType { };
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
                    if (row->at(r).bucketNode != nullptr)
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
        /**
         * Insert an empty node for the given key
         * @param key
         * @return Pointer to empty node
         */
        BucketNode* InsertEmpty(Key key)
        {
            BucketPos    pos = GetBucket(key);
            RowHead&     row = GetRow(pos)->at(LowWord(pos) % Rows);
            BucketNode** bucket = &row.bucketNode;
            BucketNode*  current = *bucket;
            BucketNode*  node = new BucketNode(key);
            bool newKey = (current == nullptr) || ((current->key != key) && (current->next == nullptr));

            // Check if we can push the node onto the front of the list
            if (newKey)
            {
                PushHead(bucket, node);
                PushKey(row, node, key);
                return node;
            }

            // Try to find the first value via the key list
            auto* keyNode = FindKey(key);
            if (keyNode != nullptr)
            {
                // Append node after the key's first value
                node->next = keyNode->first->next;
                keyNode->first->next = node;
                keyNode->count++;
                return node;
            }

            // Insert value at the front of the list
            PushHead(bucket, node);
            PushKey(row, node, key);
            return node;
        }

        void PushHead(BucketNode** bucket, BucketNode* node)
        {
            // Save next node pointer if it exists
            node->next = *bucket;
            *bucket = node;
        }

        /**
         * Push a key onto the front of a row's key list
         * @param head
         * @param node
         * @param key
         */
        void PushKey(RowHead& head, BucketNode* node, Key key)
        {
            auto keyNode = new KeyNode { };
            keyNode->key   = key;
            keyNode->next  = head.keyNode;
            keyNode->first = node;
            keyNode->count = 1;
            head.keyNode  = keyNode;
        }

        /**
         * Find the node for a given key
         * @param key
         * @return Key node pointer if found, nullptr otherwise
         */
        KeyNode* FindKey(Key key) const
        {
            BucketPos pos = GetBucket(key);
            RowType*  row = m_BucketColumns[HighWord(pos) % Columns];

            if (!row)
                return nullptr;

            KeyNode* current = row->at(LowWord(pos) % Rows).keyNode;

            while (current != nullptr)
            {
                if (current->key == key)
                    return current;
                current = current->next;
            }

            return nullptr;
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
                row = m_BucketColumns[HighWord(pos) % Columns] = new RowType { };
            return row;
        }

        void DestroyBucketList(RowHead& row)
        {
            BucketNode* node = row.bucketNode;
            KeyNode* keyNode = row.keyNode;
            while (node != nullptr)
            {
                BucketNode* lastNode = node;
                node = node->next;
                delete lastNode;
            }

            while (keyNode != nullptr)
            {
                KeyNode* lastNode = keyNode;
                keyNode = keyNode->next;
                delete lastNode;
            }
        }

        std::array<RowType*, Columns> m_BucketColumns;
    };

}
