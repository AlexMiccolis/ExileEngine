#include "Exile/Runtime/Filesystem.hpp"
#include <cstdio>

namespace Exi::Runtime
{

    FileHandle::FileHandle()
        : m_File(nullptr), m_Valid(false) { }

    FileHandle::FileHandle(std::shared_ptr<FileControl>&& file)
        : m_File(std::move(file)), m_Valid(m_File && m_File->IsOpen())
    {

    }

    FileHandle::~FileHandle()
    {

    }

    size_t FileHandle::GetSize() const
    {
        return m_Valid ? m_File->GetSize() : 0;
    }

    std::size_t FileHandle::ReadBytes(std::size_t count, void* buffer)
    {
        if (!m_Valid)
            return 0;
        auto bytes = m_File->ReadBytes(*this, count, buffer);
        m_Offset += bytes;
        return bytes;
    }

    std::size_t FileHandle::WriteBytes(std::size_t count, const void* buffer)
    {
        if (!m_Valid)
            return 0;
        auto bytes = m_File->WriteBytes(*this, count, buffer);
        m_Offset += bytes;
        return bytes;
    }

    std::string FileHandle::ReadString(std::size_t length)
    {
        const auto size = (length == 0) ? (GetSize() - m_Offset) : std::min(GetSize(), length);
        std::string str;

        str.resize(size);
        ReadBytes(size, str.data());

        return str;
    }

    bool FileHandle::WriteString(const std::string& str)
    {
        auto written = WriteBytes(str.length(), str.data());
        return written == str.length();
    }

    void FileHandle::SetEof(bool eof)
    {
        m_EndOfFile = eof;
    }

}
