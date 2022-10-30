#include <Exile/Runtime/Filesystem.hpp>
#include <utility>

namespace Exi::Runtime
{
    #pragma region VfsNode

    VfsNode::VfsNode(std::string name, std::string target)
        : m_Name(std::move(name)), m_Target(std::move(target))
    {

    }

    VfsNode* VfsNode::CreateChild(const std::string& name, const std::string& target)
    {
        auto nameFragment = PathUtils::StripSeparators(name);
        return m_Children.emplace_back(
            std::make_unique<VfsNode>(std::string(nameFragment), target)
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
        : m_Vfs("/", rootTarget.string())
    {
        auto* lua = m_Vfs.CreateChild("Lua", m_Vfs.GetTarget() + "/Lua");

        auto* entities = m_Vfs.CreateChild("Entities", m_Vfs.GetTarget() + "/Entities");
        entities->CreateChild("Objects", m_Vfs.GetTarget() + "/Entities/Objects");
    }

    Filesystem::~Filesystem()
    {

    }

    bool Filesystem::TranslatePath(const Path& virtualPath, Path& physicalPath)
    {
        std::size_t startIndex;
        std::string path = virtualPath.string();
        VfsNode* endNode = LastMatchingNode(path, startIndex, true);

        if (!endNode)
        {
            return false;
        }

        physicalPath = endNode->GetTarget() + path.substr(startIndex);
        physicalPath = physicalPath.make_preferred();
        return true;
    }

    VfsNode* Filesystem::LastMatchingNode(const std::string& path, std::size_t& indexOut, bool allowRoot)
    {
        VfsNode* node = &m_Vfs;

        indexOut = 0;

        if (!node->HasChildren())
            return allowRoot ? node : nullptr;

        std::size_t index = 0;
        auto fragment = Exi::Runtime::PathUtils::GetFirstFragment(path, index);
        while (!fragment.empty())
        {
            if (!node->HasChildren())
                break;

            VfsNode* child = node->FindChildByName(fragment);
            if (child == nullptr)
                break;

            node = child;
            indexOut = index;
            fragment = Exi::Runtime::PathUtils::GetFirstFragment(path, index);
        }

        return node;
    }

#pragma endregion
}
