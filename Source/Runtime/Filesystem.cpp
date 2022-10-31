#include <Exile/Runtime/Filesystem.hpp>
#include <utility>

namespace Exi::Runtime
{
    #pragma region VfsNode

    VfsNode::VfsNode(MountType type, std::string name, std::string target)
        : m_Type(type), m_Name(std::move(name)), m_Target(std::move(target))
    {

    }

    VfsNode* VfsNode::CreateChild(MountType type, const std::string_view& name, const std::string_view& target)
    {
        return m_Children.emplace_back(
            std::make_unique<VfsNode>(type, std::string(name), std::string(target))
        ).get();
    }

    VfsNode* VfsNode::FindChildByName(const std::string_view& name)
    {
        auto nameFragment = PathUtils::StripSeparators(name);
        for (auto& child : m_Children)
        {
            if (child->m_Name == nameFragment)
                return child.get();
        }
        return nullptr;
    }

    #pragma endregion

    #pragma region Filesystem

    Filesystem::Filesystem(const Path& rootTarget)
        : m_Vfs(VfsNode::DirectoryMount, "/", rootTarget.string()),
          m_Logger(Logger::GetLogger("Filesystem")), m_TranslationCache(1024)
    {

    }

    Filesystem::~Filesystem()
    {

    }

    bool Filesystem::MountDirectory(const Path& directory, const Path& virtualPath)
    {
        std::string dir = directory.string();
        std::string path = virtualPath.string();
        if (!std::filesystem::is_directory(directory))
        {
            m_Logger.Warn("Attempted to mount non-directory '%s'", dir.c_str());
            return false;
        }

        std::vector<std::string_view> fragments;
        PathUtils::SplitPath(path, fragments);

        std::size_t endIndex;
        VfsNode& endNode = MatchPath(path, fragments, endIndex);

        // Some part of the path doesn't exist
        if (endIndex < (fragments.size() - 1))
        {
            m_Logger.Warn("Virtual path '%s' does not exist", path.c_str());
            return false;
        }

        // Create new child node and clear the TLB
        endNode.CreateChild(VfsNode::DirectoryMount, fragments[endIndex], dir);
        m_TranslationCache.Clear();
        return true;
    }

    bool Filesystem::TranslatePath(const Path& virtualPath, Path& physicalPath)
    {
        std::string path = virtualPath.string();

        if (m_TranslationCache.Contains(path))
        {
            physicalPath = m_TranslationCache.Get(path);
            return true;
        }

        std::vector<std::string_view> fragments;
        PathUtils::SplitPath(path, fragments);

        std::size_t endIndex;
        VfsNode& endNode = MatchPath(path, fragments, endIndex);

        /*
        std::size_t startIndex;
        VfsNode& endNode = LastMatchingNode(path, startIndex);
        */

        physicalPath = endNode.GetTarget();
        for (std::size_t i = endIndex; i < fragments.size(); i++)
        {
            physicalPath /= fragments[i];
        }

        //physicalPath = endNode.GetTarget() + path.substr(startIndex);

        physicalPath = physicalPath.make_preferred();
        m_TranslationCache.Put(path, physicalPath);
        return true;
    }

    VfsNode& Filesystem::MatchPath(const std::string& path,
                                   const std::vector<std::string_view>& fragments,
                                   std::size_t& indexOut)
    {
        VfsNode* node = &m_Vfs;

        indexOut = 0;

        for (const auto& fragment : fragments)
        {
            if (!node->HasChildren())
                return *node;

            VfsNode* child = node->FindChildByName(fragment);
            if (child == nullptr)
                break;

            node = child;
            ++indexOut;
        }

        return *node;
    }

#pragma endregion
}
