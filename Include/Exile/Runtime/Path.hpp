#pragma once

#include <Exile/Runtime/API.hpp>
#include <string>
#include <vector>

namespace Exi::Runtime
{

    namespace PathUtils
    {

        /**
         * Find the first component in the given path, starting at `index`.
         * @param path Path
         * @param index Reference to index, used to store state across calls
         * @return String view of fragment if found, empty string view otherwise
         */
        static inline std::string_view GetNextFragment(const std::string_view& path, std::size_t& index)
        {
            // Find first character that isn't a directory separator
            std::size_t start = path.find_first_not_of("\\/", index);

            // End of path
            if (start == std::string::npos)
                return { path.data() + path.length(), 0 };

            // Loop until directory separator
            for (index = start; index < path.length(); index++)
            {
                auto c = path[index];
                if (c == '/' || c == '\\')
                    break;
            }

            return { path.data() + start, index - start };
        }

        /**
         * Return a view of the given string without leading or trailing
         * directory separators.
         * @param path
         * @return String view
         */
        static inline std::string_view StripSeparators(const std::string_view& path)
        {
            std::size_t index = 0;
            return GetNextFragment(path, index);
        }

        /**
         * Split a path into a vector of fragments
         * @param path
         * @param fragmentsOut
         * @return Number of fragments in the path
         */
        static inline int SplitPath(const std::string& path, std::vector<std::string_view>& fragmentsOut)
        {
            int count = 0;
            std::size_t index = 0;
            auto fragment = GetNextFragment(path, index);

            fragmentsOut.reserve(16);
            while (!fragment.empty())
            {
                fragmentsOut.push_back(fragment);
                fragment = GetNextFragment(path, index);
            }

            return fragmentsOut.size();
        }

    }

    class RUNTIME_API Path
    {
    public:
        static constexpr char DirectorySeparator = '/';

        #ifdef _WIN32
            static constexpr char AltDirectorySeparator = '\\';
        #else
            static constexpr char AltDirectorySeparator = '/';
        #endif

        Path();
        Path(const std::string& path);
        Path(const std::string_view& path) : Path(std::string(path)) { }
        Path(const char* path) : Path(std::string(path)) { }

        [[nodiscard]] bool IsAbsolute() const noexcept { return m_Attributes.Absolute; }

        [[nodiscard]] const std::string& AsString() const noexcept { return m_Path; }
        [[nodiscard]] const char* AsCString()       const noexcept { return m_Path.c_str(); }

        Path& operator/=(const Path& p)
        {
            if (m_Path.back() != DirectorySeparator)
                m_Path.push_back(DirectorySeparator);
            m_Path.append(p.AsString());
            return *this;
        }

        Path& operator/=(const std::string& s)
        {
            return (*this) /= Path(s);
        }

        Path& operator/=(const std::string_view& s)
        {
            return (*this) /= Path(s);
        }

        friend bool operator==(const Path& a, const Path& b)
        { return a.m_Path == b.m_Path; }

        friend bool operator!=(const Path& a, const Path& b)
        { return !(a.m_Path == b.m_Path); }
    private:
        std::string m_Path; // Path string
        uint32_t m_Fragments; // Number of fragments in path
        struct
        {
            uint32_t Absolute : 1; // 1 if the path is absolute
        } m_Attributes;
    };

}
