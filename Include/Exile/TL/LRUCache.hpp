#pragma once

#include <deque>
#include <unordered_map>

namespace Exi::TL
{

    template <class K, class V>
    class LRUCache
    {
    public:
        using Key   = K;
        using Value = V;

        LRUCache(size_t capacity)
                : m_Capacity(capacity)
        { }

        /** Check if the cache contains a key */
        bool Contains(const Key& key) const
        {
            return m_Cache.contains(key);
        }

        /** Get a value from the cache */
        const Value& Get(const Key& key)
        {
            auto it = std::find(m_Keys.cbegin(), m_Keys.cend(), key);
            m_Keys.erase(it);
            m_Keys.push_front(key);
            return m_Cache.at(key);
        }

        /** Move a value into the cache */
        void Put(const Key& key, Value&& value)
        {
            if (!Contains(key))
            {
                if (m_Keys.size() >= m_Capacity)
                {
                    m_Cache.erase(m_Keys.back());
                    m_Keys.pop_back();
                }
            }
            else
            {
                auto it = std::find(m_Keys.cbegin(), m_Keys.cend(), key);
                m_Keys.erase(it);
                m_Cache.erase(key);
            }

            m_Keys.push_front(key);

            Key keyCopy = key;
            m_Cache.emplace(std::move(keyCopy), std::move(value));
        }

        /** Copy a value into the cache */
        void Put(const Key& key, const Value& value)
        {
            Put(key, std::move(Value(value)));
        }

        /** Clear the cache */
        void Clear()
        {
            m_Keys.clear();
            m_Cache.clear();
        }

        /** Get current size of the cache in items */
        [[nodiscard]] std::size_t Size() const { return m_Keys.size(); }

        /** Get current estimated size of the cache in bytes */
        [[nodiscard]] std::size_t MemorySize() const
        {
            return sizeof(LRUCache) +
                (m_Keys.size() * sizeof(Key)) +
                (m_Cache.size() * sizeof(std::pair<Key, Value>));
        }
    private:
        std::deque<Key> m_Keys;
        std::unordered_map<Key, Value> m_Cache;
        std::size_t m_Capacity;
    };

}