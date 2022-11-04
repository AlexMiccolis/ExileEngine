#include <Exile/Runtime/Filesystem.hpp>
#include <filesystem>
#include <utility>
#include <cstdio>

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
    Filesystem::Filesystem(const Path& rootDirectory)
        : m_Vfs(VfsNode::DirectoryMount, "/", rootDirectory.AsString()),
          m_Logger(Logger::GetLogger("Filesystem")), m_TranslationCache(1024)
    {

    }

    Filesystem::Filesystem()
        : Filesystem(std::filesystem::current_path().string())
    {

    }

    Filesystem::~Filesystem()
    {

    }

    FileHandle Filesystem::Open(const Path& path, OpenMode mode)
    {
        std::unique_lock lock(m_FileMutex);
        Path physicalPath;

        if (!TranslatePath(path, physicalPath))
            return { };

        // Find an FCB with a matching path
        auto it = std::find_if(m_Files.begin(), m_Files.end(),
            [=](const auto& v) -> bool
            { return v->GetPhysicalPath() == physicalPath; }
        );
        std::shared_ptr<FileControl> fcb;

        if (it == m_Files.end())
        {
            // No FCB found for this path, create one
            fcb = m_Files.emplace_back(new FileControl(*this, path, physicalPath, mode));
        }
        else
        {
            fcb = *it;

            if (mode == ReadWrite && fcb->GetOpenMode() == Read)
            {
                if (!fcb->ReopenAs(ReadWrite))
                {
                    m_Logger.Error("Failed to re-open '%s' as read/write", physicalPath.AsCString());
                    return { };
                }
            }
            else if (!fcb->CanOpenWith(mode))
            {
                m_Logger.Warn("Attempted to open read-only file '%s' as writable", physicalPath.AsCString());
                return { };
            }
        }

        return FileHandle(std::move(fcb));
    }

    bool Filesystem::MountDirectory(const Path& directory, const Path& virtualPath)
    {
        const auto& dir = directory.AsString();
        const auto& path = virtualPath.AsString();
        if (!std::filesystem::is_directory(directory.AsString()))
        {
            m_Logger.Warn("Attempted to mount non-directory '%s'", dir.c_str());
            return false;
        }

        std::vector<std::string_view> fragments;
        PathUtils::SplitPath(path, fragments);

        // Get exclusive access to the VFS because we're about to modify it
        std::unique_lock vfsLock(m_VfsMutex);
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
        std::unique_lock cacheLock(m_CacheMutex);
        const auto& path = virtualPath.AsString();

        if (m_TranslationCache.Contains(path))
        {
            physicalPath = m_TranslationCache.Get(path);
            return true;
        }

        std::vector<std::string_view> fragments;
        PathUtils::SplitPath(path, fragments);

        std::size_t endIndex;
        VfsNode& endNode = MatchPath(path, fragments, endIndex);

        physicalPath = endNode.GetTarget();
        if (physicalPath.AsString().empty())
            return false;

        for (std::size_t i = endIndex; i < fragments.size(); i++)
        {
            physicalPath /= fragments[i];
        }

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
