#include <Exile/Runtime/Path.hpp>

namespace Exi::Runtime
{

    Path::Path(const std::string& path)
    {
        if (path.empty())
            return;

        std::vector<std::string_view> fragments;
        m_Fragments = PathUtils::SplitPath(path, fragments);
        m_Path.reserve(path.size());

        if ((path.front() == DirectorySeparator) || (path.front() == AltDirectorySeparator))
        {
            m_Path.push_back(DirectorySeparator);
            m_Attributes.Absolute = true;
        }

        for (auto i = 0; i < m_Fragments; i++)
        {
            const auto& s = fragments.at(i);
            m_Path.append(s);

            if (i == m_Fragments - 1)
            {
                if ((path.back() != DirectorySeparator) && (path.back() != AltDirectorySeparator))
                    break;
            }
            m_Path.push_back(DirectorySeparator);
        }
    }

    std::string Path::GetExtension() const
    {
        auto file = GetFile();

        if (file.empty())
            return file;

        auto ext = file.find_first_of('.');
        if (ext == std::string::npos || ext == file.length() - 1)
            return { };

        return { file, ext + 1 };
    }

    std::string Path::GetFileName() const
    {
        auto file = GetFile();

        if (file.empty())
            return file;

        auto ext = file.find_first_of('.');
        if (ext == 0)
            return { };
        else if (ext == std::string::npos)
            return file;

        return { file, 0, ext };
    }

    std::string Path::GetFile() const
    {
        constexpr char chars[] = { DirectorySeparator, AltDirectorySeparator, 0 };
        auto lastSeparator = m_Path.find_last_of(chars);

        if (lastSeparator == (m_Path.length() - 1))
            return { };

        if (lastSeparator == std::string::npos)
            lastSeparator = 0;

        return { m_Path, lastSeparator + 1 };
    }

}
