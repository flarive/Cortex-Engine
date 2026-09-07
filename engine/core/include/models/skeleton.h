#pragma once

#include "../misc/noncopyable.h"

#include "../common_defines.h"

#include <unordered_map>

namespace engine
{
    struct SkeletonBone final
    {
        std::string name;
        int parentIndex;               // -1 if root
        glm::mat4 localBindTransform;  // bind pose
        glm::mat4 offset;              // inverse bind pose
    };

    class Skeleton final // : public NonCopyable
    {
    public:
        std::vector<SkeletonBone> m_skeletonBones{};     // flat list
        int m_skeletonRootIndex{ -1 };

        // Lookup tables
        std::unordered_map<std::string, int> nameToIndex{}; // std::map<std::string, BoneInfo> m_boneInfoMap{}; // instead !!!!!!!!

        const SkeletonBone& getBone(int index) const { return m_skeletonBones[index]; }
        int getIndex(const std::string& name) const
        {
            auto it = nameToIndex.find(name);
            return (it == nameToIndex.end()) ? -1 : it->second;
        }
    };
}