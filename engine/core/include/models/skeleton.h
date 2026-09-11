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
        Skeleton();
        ~Skeleton();

        std::vector<SkeletonBone>& getSkeletonBones() { return m_skeletonBones; }

        // non-const: used when building / editing the skeleton
        SkeletonBone& getSkeletonBone(int index);

        // const: used when reading the skeleton (render, animation, etc.)
        const SkeletonBone& getSkeletonBone(int index) const;

        int addSkeletonBone(const SkeletonBone& bone);
        void setSkeletonBone(int index, const SkeletonBone& bone);

        SkeletonBone* findSkeletonBone(const std::string& name);
        const SkeletonBone* findSkeletonBone(const std::string& name) const;
        

        unsigned int getSkeletonBoneCount() const;
		

        int getSkeletonRootIndex() const { return m_skeletonRootIndex; }
        void setSkeletonRootIndex(int index) { m_skeletonRootIndex = index; }


        // faster lookup, faster than vector
        int getSkeletonNameIndex(const std::string& name) const;

    private:
        std::vector<SkeletonBone> m_skeletonBones{}; // flat list
        int m_skeletonRootIndex{ -1 };

        // Lookup tables (for performance)
        std::unordered_map<std::string, int> m_nameToIndex{};
    };
}