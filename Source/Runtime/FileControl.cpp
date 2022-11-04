#include <Exile/Runtime/Filesystem.hpp>
#include <cstdio>
#include <cassert>

namespace Exi::Runtime
{

    const char* FileControl::OpenModeToString(int openMode)
    {
        switch (openMode)
        {
            case Filesystem::Read:
            case Filesystem::ReadOnly:
                return s_Read;
            case Filesystem::ReadWrite:
                return s_ReadWrite;
            case Filesystem::WriteTruncate:
                return s_WriteTruncate;
            default:
                break;
        }
        assert(0);
        return s_Read;
    }

    FileControl::FileControl(class Filesystem& filesystem,
                             Path virtualPath,
                             Path physicalPath,
                             int openMode)
        : m_Filesystem(filesystem), m_VirtualPath(std::move(virtualPath)),
          m_PhysicalPath(std::move(physicalPath)), m_OpenMode(openMode)
    {
        const bool rw = (openMode == Filesystem::ReadWrite);
        const bool trunc = (openMode == Filesystem::WriteTruncate);

        m_File = fopen(m_PhysicalPath.AsCString(), OpenModeToString(openMode));
        if (m_File == nullptr)
            return;

        m_Readable = true;
        m_Writable = rw || trunc;

        // TODO: 64-bit file offsets
        fseek(m_File, 0, SEEK_END);
        m_Size = ftell(m_File);
        rewind(m_File);
    }

    FileControl::~FileControl()
    {
        if (!m_MemoryMapped && m_Readable)
            fclose(m_File);
    }

    bool FileControl::ReopenAs(int openMode)
    {
        std::unique_lock lock(m_Mutex);
        if (!m_MemoryMapped && m_Readable)
        {
            FILE* file = fopen(m_PhysicalPath.AsCString(), OpenModeToString(openMode));
            if (!file)
            {
                return false;
            }
            fclose(m_File);
            m_File = file;
            m_OpenMode = openMode;
        }
        return true;
    }

    bool FileControl::CanOpenWith(int openMode) const
    {
        if (openMode == Filesystem::Read || openMode == Filesystem::ReadOnly)
            return m_Readable;
        return m_Readable && m_Writable;
    }

    std::size_t FileControl::ReadBytes(FileHandle& handle, std::size_t count, void* buffer)
    {
        std::unique_lock lock(m_Mutex);
        std::size_t bytes = 0;
        if (m_Readable && !m_MemoryMapped)
        {
            fseek(m_File, static_cast<long>(handle.GetOffset()), SEEK_SET);
            bytes = fread(buffer, 1, count, m_File);
            if (bytes != count)
            {
                if (feof(m_File) != 0)
                    handle.SetEof(true);
            }
            else
            {
                handle.SetEof(false);
            }
        }
        return bytes;
    }

    std::size_t FileControl::WriteBytes(FileHandle& handle, std::size_t count, const void* buffer)
    {
        std::unique_lock lock(m_Mutex);
        std::size_t bytes = 0;
        if (m_Readable && !m_MemoryMapped)
        {
            auto offset = static_cast<long>(handle.GetOffset());

            fseek(m_File, offset, SEEK_SET);
            bytes = fwrite(buffer, 1, count, m_File);

            if (offset + bytes > m_Size)
                m_Size += (offset + bytes) - m_Size;
        }
        return bytes;
    }

}
