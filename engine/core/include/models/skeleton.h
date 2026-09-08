#pragma once

#include "../misc/noncopyable.h"

#include "../common_defines.h"
#include "bone.h"

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

    class Skeleton final //: public NonCopyable
    {
    public:
        std::vector<SkeletonBone> m_skeletonBones{};     // flat list
        int m_skeletonRootIndex{ -1 };

        
        // a legacy Assimp-style lookup table you no longer need (TO BE REMOVED)
        // use simple std::unordered_map<std::string, int> m_nameToIndex; instead
        std::map<std::string, BoneInfo> m_boneInfoMap{};

        const SkeletonBone& getBone(int index) const { return m_skeletonBones[index]; }

        std::map<std::string, BoneInfo>& getBoneInfoMap() { return m_boneInfoMap; }

		unsigned int& getBoneCount() { return m_boneCounter; }
		void setBoneCount(unsigned int count) { m_boneCounter = count; }


        // Lookup tables
        //std::unordered_map<std::string, int> nameToIndex;

        //const SkeletonBone& getBone(int index) const { return bones[index]; }
        //int getIndex(const std::string& name) const
        //{
        //    auto it = nameToIndex.find(name);
        //    return (it == nameToIndex.end()) ? -1 : it->second;
        //}
  

    private:
		unsigned int m_boneCounter{};
    };
}