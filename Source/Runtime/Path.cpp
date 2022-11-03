#include <Exile/Runtime/Path.hpp>

namespace Exi::Runtime
{

    Path::Path()
    {

    }

    Path::Path(const std::string& path)
        : m_Attributes { }
    {
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

}
