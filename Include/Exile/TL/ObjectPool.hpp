#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <new>

namespace Exi::TL
{
    /**
     * Page-aligned block of memory to hold objects for an ObjectPool
     * @tparam T       Object type
     * @tparam Objects Object count
     */
    template <class T, std::size_t Objects>
    class alignas(4096) ObjectBlock
    {
    public:
        using Object = T;
        static constexpr std::size_t ObjectCount = Objects;
        static constexpr std::size_t ObjectSize  = sizeof(Object);
        static constexpr std::size_t TotalSize   = ObjectCount * ObjectSize;
        static constexpr std::size_t BlockSize   = sizeof(ObjectBlock);

        /**
         * Check if this block owns an object pointer
         * @param ptr
         * @return True if this block owns the pointer, false otherwise
         */
        bool Owns(Object* ptr) const
        {
            std::size_t val = reinterpret_cast<std::size_t>(ptr);
            return val >= reinterpret_cast<std::size_t>(m_Storage.data())
                && val <= reinterpret_cast<std::size_t>(m_Storage.data() + m_Storage.size());
        }

        /**
         * Construct an object in the block
         * @tparam Args
         * @param args
         * @return Object pointer
         */
        template <class... Args>
        Object* Get(Args&& ...args)
        {
            for (std::size_t i = 0; i < ObjectCount; i++)
            {
                if (!m_FreeMap.test(i))
                {
                    Object* ptr = reinterpret_cast<Object*>(m_Storage.data() + (i * sizeof(Object)));
                    m_FreeMap.flip(i);
                    --m_Free;
                    return new (ptr) Object (std::forward<Args>(args)...);
                }
            }

            return nullptr;
        }

        /**
         * Release an object back to the block
         * @param ptr Object pointer
         * @return True if the object was successfully released, false otherwise
         */
        bool Release(Object* ptr)
        {
            if (!Owns(ptr))
                return false;

            auto diff = reinterpret_cast<std::ptrdiff_t>(reinterpret_cast<uint8_t*>(ptr) - m_Storage.data());
            auto index = diff / ObjectSize;

            assert(m_FreeMap.test(index));

            std::destroy_at(ptr);
            m_FreeMap.reset(index);
            ++m_Free;
            return true;
        }

        [[nodiscard]] bool Empty() const { return m_Free == 0; }

    private:
        std::array<uint8_t, TotalSize> m_Storage;
        std::bitset<ObjectCount> m_FreeMap;
        std::size_t m_Free = ObjectCount;
    };

    /**
     * Object pool backed by an arena allocator
     * @tparam T
     * @tparam PerBlock
     */
    template <class T, std::size_t PerBlock = (4096 - 128) / sizeof(T)>
    class alignas(64) ObjectPool
    {
    public:
        static constexpr std::size_t ObjectsPerBlock = PerBlock;

        using Object = T;
        using Block = ObjectBlock<Object, ObjectsPerBlock>;
        using BlockPointer = Block*;

        /**
         * Construct an object from the object pool
         * @tparam Args
         * @param args
         * @return Object pointer
         */
        template <class... Args>
        Object* Get(Args&& ...args)
        {
            /* Loop through blocks and try to get an object */
            for (auto* block : m_Blocks)
            {
                if (block->Empty())
                    continue;

                Object* ptr = block->template Get(std::forward<Args>(args)...);
                if (ptr != nullptr)
                    return ptr;
            }

            /* No free objects, get a new block */
            Block* block = AddBlock();
            return block->template Get(std::forward<Args>(args)...);
        }

        /**
         * Release an object back to the object pool
         * @param ptr Object pointer
         * @return True if the object was successfully released, false otherwise
         */
        bool Release(Object* ptr)
        {
            if (ptr == nullptr)
                return false;

            for (auto& block : m_Blocks)
            {
                if (block->Release(ptr))
                    return true;
            }

            return false;
        }

        ObjectPool() { AddBlock(); }
        ~ObjectPool()
        {
            for (auto* block : m_Blocks)
                delete block;
        }
    private:
        Block* AddBlock()
        {
            return m_Blocks.emplace_back(new Block);
        }

        std::vector<BlockPointer> m_Blocks;
    };

}
