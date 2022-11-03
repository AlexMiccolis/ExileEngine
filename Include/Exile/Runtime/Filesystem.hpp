#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Path.hpp>
#include <Exile/Runtime/Logger.hpp>
#include <Exile/TL/LRUCache.hpp>
#include <cstddef>
#include <list>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <shared_mutex>

namespace Exi::Runtime
{

    /** Node within a Virtual Filesystem tree */
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

    /** Shared control state for an open file */
    class RUNTIME_API FileControl
    {
    public:
        [[nodiscard]] const Path& GetVirtualPath() const { return m_VirtualPath; }
        [[nodiscard]] const Path& GetPhysicalPath() const { return m_PhysicalPath; }
        [[nodiscard]] size_t GetSize() const { return m_Size; }
        [[nodiscard]] bool IsOpen() const { return m_Readable; }
        [[nodiscard]] bool IsWritable() const { return m_Writable; }
        [[nodiscard]] bool IsMemoryMapped() const { return m_MemoryMapped; }

        [[nodiscard]] bool CanOpenWith(int openMode) const;

        ~FileControl();
    private:
        friend class Filesystem;

        FileControl(class Filesystem& filesystem,
                    Path virtualPath,
                    Path physicalPath,
                    int openMode);

        /** Reference to parent filesystem */
        class Filesystem& m_Filesystem;

        /** Virtual path this file was initially opened with, mainly for debugging purposes */
        const Path m_VirtualPath;

        /** On-disk path of the file */
        const Path m_PhysicalPath;

        /** Size of the file */
        std::atomic_size_t m_Size;

        union
        {
            /** File pointer, used if m_MemoryMapped is false */
            FILE* m_File;

            /** Mapped memory, used if m_MemoryMapped is true */
            void* m_Memory;
        };

        struct
        {
            bool m_Readable     : 1;
            bool m_Writable     : 1;
            bool m_MemoryMapped : 1;
        };

        /** Mutex to keep file operations from imploding when threaded */
        mutable std::shared_mutex m_FileMutex;
    };

    /**
     * Handle to an open file.
     * The file is automatically closed when all handles go out of scope.
     * Copy constructor is removed to prevent unintentional copies from being made.
     */
    class RUNTIME_API FileHandle
    {
    public:
        FileHandle(FileHandle&&) = default;
        ~FileHandle();

        FileHandle(const FileHandle& handle) = delete;
        void operator=(const FileHandle& handle) = delete;

    private:
        friend class Filesystem;

        FileHandle(std::shared_ptr<FileControl>&& file);

        std::shared_ptr<FileControl> m_File;
        size_t m_Offset;
    };

    class RUNTIME_API Filesystem
    {
    public:
        Filesystem(const Path& rootTarget);
        ~Filesystem();

        /** File open modes */
        enum OpenMode
        {
            /** Open a file only for reading */
            ReadOnly,

            /** Open a file for reading and writing, create if it doesn't exist */
            ReadWrite,

            /** Open a file for reading and writing, truncate if it exists */
            WriteTruncate
        };

        /**
         * Open a file with the specified mode
         * @param path
         * @param mode
         * @return Handle to file
         */
        FileHandle Open(const Path& path, OpenMode mode = ReadOnly);

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
        std::list<std::shared_ptr<FileControl>> m_Files;
    };

}
