#pragma once

#include <Exile/Runtime/API.hpp>
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
        static inline std::string_view GetFirstFragment(const std::string_view& path, std::size_t& index)
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
            return GetFirstFragment(path, index);
        }

    }

    class RUNTIME_API VfsNode
    {
    public:
        VfsNode(std::string name, std::string target);

        VfsNode* CreateChild(const std::string& name, const std::string& target);
        VfsNode* FindChildByName(const std::string_view& name);

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] const std::string& GetTarget() const { return m_Target; }
        [[nodiscard]] bool HasChildren() const { return !m_Children.empty(); }
    private:
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
         * Translate a virtual path to a physical path
         * @param virtualPath Virtual path to translate
         * @param physicalPath Reference to path that will contain the physical path
         * @return True if the translation was successful, false otherwise
         */
        bool TranslatePath(const Path& virtualPath, Path& physicalPath);
    private:
        VfsNode* LastMatchingNode(const std::string& path, std::size_t& indexOut, bool allowRoot = false);

        VfsNode m_Vfs;
    };

}
