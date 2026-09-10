#include "../../include/models/skeleton.h"

// non-const: used when building / editing the skeleton
engine::SkeletonBone& engine::Skeleton::getSkeletonBone(int index)
{
    return m_skeletonBones.at(index);
}

// const: used when reading the skeleton (render, animation, etc.)
const engine::SkeletonBone& engine::Skeleton::getSkeletonBone(int index) const
{
    return m_skeletonBones.at(index);
}


int engine::Skeleton::addSkeletonBone(const SkeletonBone& bone)
{
    int index = static_cast<int>(m_skeletonBones.size());
    m_skeletonBones.push_back(bone);
    m_nameToIndex[bone.name] = index;
    return index;
}


void engine::Skeleton::setSkeletonBone(int index, const SkeletonBone& bone)
{
    m_skeletonBones[index] = bone;
    m_nameToIndex[bone.name] = index;
}

engine::SkeletonBone* engine::Skeleton::findSkeletonBone(const std::string& name)
{
    auto it = m_nameToIndex.find(name);
    if (it == m_nameToIndex.end())
        return nullptr;

    return &m_skeletonBones[it->second];
}

const engine::SkeletonBone* engine::Skeleton::findSkeletonBone(const std::string& name) const
{
    auto it = m_nameToIndex.find(name);
    if (it == m_nameToIndex.end())
        return nullptr;

    return &m_skeletonBones[it->second];
}

int engine::Skeleton::getSkeletonNameIndex(const std::string& name) const
{
    auto it = m_nameToIndex.find(name);
    return (it == m_nameToIndex.end()) ? -1 : it->second;
}

unsigned int engine::Skeleton::getSkeletonBoneCount() const
{
    return static_cast<unsigned int>(m_skeletonBones.size());
}
