#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Path.hpp>
#include <Exile/Runtime/Logger.hpp>
#include <Exile/TL/LRUCache.hpp>
#include <list>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace Exi::Runtime
{

    /**
     * Node within a Virtual Filesystem tree
     */
    class RUNTIME_API VfsNode
    {
    public:
        enum MountType
        {
            DirectoryMount,
            FileMount
        };

        VfsNode(MountType type, std::string name, std::string target);

        VfsNode* CreateChild(MountType type, const std::string_view& name, const std::string_view& target);
        VfsNode* FindChildByName(const std::string_view& name);

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] const std::string& GetTarget() const { return m_Target; }
        [[nodiscard]] bool HasChildren() const { return !m_Children.empty(); }
    private:
        MountType m_Type;
        std::string m_Name;
        std::string m_Target;
        std::vector<std::unique_ptr<VfsNode>> m_Children;
    };

    class RUNTIME_API Filesystem
    {
    public:
        struct RUNTIME_API FileBits
        {
            uint8_t Readable : 1;  // 1 if the file is readable
            uint8_t Writable : 1;  // 1 if the file is writable
        };

        /**
         * Control data for an open file
         */
        class RUNTIME_API FileControlBlock
        {
        public:
            FileControlBlock(class Filesystem& fs, Path path, bool writable);
            ~FileControlBlock();

            [[nodiscard]] bool IsReadable() const noexcept { return m_Bits.Readable; }
            [[nodiscard]] bool IsWritable() const noexcept { return m_Bits.Writable; }
            [[nodiscard]] const Path& GetPhysicalPath() const noexcept { return m_PhysicalPath; }
        private:
            class Filesystem& m_Filesystem; // Parent filesystem
            Path m_PhysicalPath; // Disk path for the file
            FILE* m_File; // File pointer if relevant
            FileBits m_Bits; // Status bits
        };

        using FcbPointer = std::shared_ptr<FileControlBlock>;

        class RUNTIME_API FileHandle
        {
        public:
            ~FileHandle();

            [[nodiscard]] bool IsReadable() const noexcept { return m_Bits.Readable; }
            [[nodiscard]] bool IsWritable() const noexcept { return m_Bits.Writable; }
        private:
            friend class Filesystem;

            FileHandle(FcbPointer&& fcb, FileBits bits);

            FcbPointer m_Fcb; // Pointer to file control block
            std::size_t m_Offset; // File stream offset
            FileBits m_Bits; // Status bits
        };

        Filesystem(const Path& rootTarget);
        ~Filesystem();

        FileHandle Open(const Path& path, bool writable = false);

        /**
         * Mount a directory at the specified virtual path
         * @param directory
         * @param virtualPath
         * @return True if the directory was successfully mounted, false otherwise
         */
        bool MountDirectory(const Path& directory, const Path& virtualPath);

        /**
         * Translate a virtual path to a physical path
         * @param virtualPath Virtual path to translate
         * @param physicalPath Reference to path that will contain the physical path
         * @return True if the translation was successful, false otherwise
         */
        bool TranslatePath(const Path& virtualPath, Path& physicalPath);
    private:
        /**
         * Return the VFS node that matches the most fragments of a path
         * @param path
         * @param indexOut
         * @return
         */
        VfsNode& MatchPath(const std::string& path,
                           const std::vector<std::string_view>& fragments,
                           std::size_t& indexOut);

        Logger& m_Logger;

        std::shared_mutex m_VfsMutex;
        VfsNode m_Vfs;
        TL::LRUCache<std::string, Path> m_TranslationCache;

        std::shared_mutex m_FileMutex;
        std::list<FcbPointer> m_FcbList;
    };

}
