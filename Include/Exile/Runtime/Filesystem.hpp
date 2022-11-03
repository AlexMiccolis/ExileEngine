#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Path.hpp>
#include <Exile/Runtime/Logger.hpp>
#include <Exile/TL/LRUCache.hpp>
#include <cstddef>
#include <concepts>
#include <list>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <cassert>

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
        [[nodiscard]] int GetOpenMode() const { return m_OpenMode; }

        [[nodiscard]] bool CanOpenWith(int openMode) const;

        /**
         * Read `count` bytes from the file handle's offset into `buffer`.
         * @param handle File handle
         * @param count Number of bytes to read
         * @param buffer Destination buffer
         * @return Number of bytes read
         */
        std::size_t ReadBytes(class FileHandle& handle, std::size_t count, void* buffer);

        /**
         * Write `count` bytes from `buffer` to the file at the handle's offset
         * @param handle File handle
         * @param count Number of bytes to write
         * @param buffer Source buffer
         * @return Number of bytes written
         */
        std::size_t WriteBytes(class FileHandle& handle, std::size_t count, const void* buffer);

        ~FileControl();
    private:
        friend class Filesystem;

        static constexpr const char* s_Read = "rb";
        static constexpr const char* s_ReadWrite = "r+b";
        static constexpr const char* s_WriteTruncate = "w+b";

        static const char* OpenModeToString(int openMode);

        FileControl(class Filesystem& filesystem,
                    Path virtualPath,
                    Path physicalPath,
                    int openMode);

        bool ReopenAs(int openMode);

        /** Reference to parent filesystem */
        class Filesystem& m_Filesystem;

        /** Virtual path this file was initially opened with, mainly for debugging purposes */
        const Path m_VirtualPath;

        /** On-disk path of the file */
        const Path m_PhysicalPath;

        /** Last OpenMode of file */
        int m_OpenMode;

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
            bool m_Readable     : 1 = false;
            bool m_Writable     : 1 = false;
            bool m_MemoryMapped : 1 = false;
        };

        /** Mutex to keep file operations from imploding when threaded */
        mutable std::shared_mutex m_Mutex;
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

        [[nodiscard]] size_t GetOffset() const { return m_Offset; }
        [[nodiscard]] bool EndOfFile() const { return m_EndOfFile; }
        [[nodiscard]] bool IsValid() const { return m_Valid; }

        /**
         * Read a trivially copyable type and increase the internal offset by its size
         * @tparam T
         * @param out
         * @return True if the value was read, false otherwise
         */
        template <typename T> requires std::is_trivially_copyable_v<T>
        bool Read(T& out)
        {
            assert(m_Valid);
            auto bytes = m_File->ReadBytes(*this, sizeof(T), &out);
            m_Offset += bytes;
            return !m_EndOfFile;
        }

        /**
         * Write a trivially copyable type and increase the internal offset by its size
         * @tparam T
         * @param val
         * @return True if the value was written, false otherwise
         */
        template <typename T> requires std::is_trivially_copyable_v<T>
        bool Write(const T& val)
        {
            assert(m_Valid);
            auto bytes = m_File->WriteBytes(*this, sizeof(T), &val);
            m_Offset += bytes;
            return bytes == sizeof(T);
        }

        explicit operator bool() const { return m_Valid; }

        FileHandle(const FileHandle& handle) = delete;
        void operator=(const FileHandle& handle) = delete;

    private:
        friend class Filesystem;
        friend class FileControl;

        FileHandle(std::shared_ptr<FileControl>&& file);
        void SetEof(bool eof);

        std::shared_ptr<FileControl> m_File;
        size_t m_Offset = 0;
        bool m_EndOfFile = false;
        bool m_Valid = false;
    };

    class RUNTIME_API Filesystem
    {
    public:
        Filesystem(const Path& rootTarget);
        ~Filesystem();

        /** File open modes */
        enum OpenMode
        {
            /**
             * Open a file for reading, but allow it to be promoted
             * to ReadWrite or demoted to ReadOnly access if necessary
             */
            Read,

            /** Open a file only for reading, do not allow it to be promoted to ReadWrite */
            ReadOnly,

            /** Open a file for reading and writing only if it exists */
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
        FileHandle Open(const Path& path, OpenMode mode = Read);

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
