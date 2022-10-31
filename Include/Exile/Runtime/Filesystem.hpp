#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Logger.hpp>
#include <Exile/TL/LRUCache.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>

namespace Exi::Runtime
{

    using Path = std::filesystem::path;

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
        Filesystem(const Path& rootTarget);
        ~Filesystem();

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

        VfsNode m_Vfs;
        Logger& m_Logger;
        TL::LRUCache<std::string, Path> m_TranslationCache;
    };

}
