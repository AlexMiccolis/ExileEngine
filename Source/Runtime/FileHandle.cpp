#include <Exile/Runtime/Filesystem.hpp>
#include <cstdio>

namespace Exi::Runtime
{

    FileHandle::FileHandle(std::shared_ptr<FileControl>&& file)
        : m_File(std::move(file)), m_Valid(m_File)
    {

    }

    FileHandle::~FileHandle()
    {

    }

    void FileHandle::SetEof(bool eof)
    {
        m_EndOfFile = eof;
    }

}
