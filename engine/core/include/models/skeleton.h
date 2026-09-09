#pragma once

#include "../misc/noncopyable.h"

#include "../common_defines.h"
#include "bone.h"

#include <unordered_map>

namespace engine
{
    /// <summary>
    /// STATIC RIG DATA (never changes)
    /// Not the same as Bone
    /// </summary>
    struct SkeletonBone final
    {
        std::string name;
        int parentIndex;               // -1 if root
        glm::mat4 localBindTransform;  // bind pose
        glm::mat4 offset;              // inverse bind pose
    };

    class Skeleton final : public NonCopyable
    {
    public:
        
        std::vector<SkeletonBone>& getBones() { return m_skeletonBones; }
        const SkeletonBone& getBone(int index) const { return m_skeletonBones[index]; }

        void addBone(const SkeletonBone& bone)
        {
            int index = static_cast<int>(m_skeletonBones.size());
            m_skeletonBones.push_back(bone);
            m_nameToIndex[bone.name] = index;
        }

        void setBone(int index, const SkeletonBone& bone)
        {
            m_skeletonBones[index] = bone;
            m_nameToIndex[bone.name] = index;
        }
        


		

        std::map<std::string, BoneInfo>& getBoneInfoMap() { return m_boneInfoMap; }

		unsigned int getBoneCount() const { return static_cast<unsigned int>(m_skeletonBones.size()); }
		


        

        
        int getNameIndex(const std::string& name) const
        {
            auto it = m_nameToIndex.find(name);
            return (it == m_nameToIndex.end()) ? -1 : it->second;
        }



        

        int getRootIndex() const { return m_skeletonRootIndex; }
        void setRootIndex(int index) { m_skeletonRootIndex = index; }

        

        void reserveNameToIndex(unsigned int size)
        {
            m_nameToIndex.reserve(size);
        }

        void setNameToIndexValue(const std::string& key, int value)
        {
            m_nameToIndex[key] = value;
        }
  

    private:
        std::vector<SkeletonBone> m_skeletonBones{};     // flat list
        int m_skeletonRootIndex{ -1 };

        // a legacy Assimp-style lookup table you no longer need (TO BE REMOVED)
        // use simple std::unordered_map<std::string, int> m_nameToIndex; instead
        std::map<std::string, BoneInfo> m_boneInfoMap{};

        // Lookup tables
        std::unordered_map<std::string, int> m_nameToIndex;
    };
}