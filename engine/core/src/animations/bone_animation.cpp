#include "../../include/animations/bone_animation.h"

#include "../../include/managers/log_manager.h"


#include <assimp/importer.hpp>
#include <assimp/postprocess.h>

engine::BoneAnimation::BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor)
	: Animation(animationName, model, speedFactor), m_filepath(animationPath)
{
	logger.trace("BoneAnimation constructor called");

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];

	m_ticksPerSecond = static_cast<unsigned int>(animation->mTicksPerSecond > 0 ? animation->mTicksPerSecond : 1000.0); // mTicksPerSecond = 1000 => ticks are in milliseconds
	m_duration = static_cast<float>(animation->mDuration); // duration in ticks
	m_durationInSeconds = m_duration / m_ticksPerSecond;
	m_desiredFPS = computeFPS(animation); // usually 30 FPS for mixamo animations
	m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);

	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	readHierarchyData(m_rootNode, scene->mRootNode);
	readMissingBones(animation, *model.get());
}

engine::Bone* engine::BoneAnimation::findBone(const std::string& name)
{
	auto iter = std::find_if(m_bones.begin(), m_bones.end(),
		[&](const Bone& bone)
		{
			return bone.getBoneName() == name;
		}
	);
	if (iter == m_bones.end()) return nullptr;
	else return &(*iter);
}

void engine::BoneAnimation::readMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.getBoneInfoMap(); //getting m_boneInfoMap from Model class
	int boneCount = model.getBoneCount(); //getting the m_boneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		m_bones.push_back(Bone(channel->mNodeName.data,	boneInfoMap[channel->mNodeName.data].id, channel));
	}

	m_boneInfoMap = boneInfoMap;
}

void engine::BoneAnimation::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

// ensure anim was exported at 30 FPS (should be always the case for mixamo anims)
// should be 0, 33.3333, 66.6667, 100...
// 33.33 ms = 1/30s
unsigned int engine::BoneAnimation::computeFPS(const aiAnimation* anim)
{
	if (anim->mNumChannels == 0)
		return 0;

	const aiNodeAnim* channel = anim->mChannels[0];

	if (channel->mNumPositionKeys < 2)
		return 0;

	double t0 = channel->mPositionKeys[0].mTime;
	double t1 = channel->mPositionKeys[1].mTime;

	double deltaTicks = t1 - t0;

	double ticksPerSecond = anim->mTicksPerSecond;
	if (ticksPerSecond <= 0.0)
		ticksPerSecond = 1000.0; // Assimp fallback for FBX

	double deltaSeconds = deltaTicks / ticksPerSecond;


	//const aiNodeAnim* channel = animation->mChannels[0];
	//for (int i = 0; i < channel->mNumPositionKeys; i++) {
	//	std::cout << channel->mPositionKeys[i].mTime << std::endl;
	//}

	return static_cast<unsigned int>(std::round(1.0 / deltaSeconds));
}

engine::BoneAnimation::~BoneAnimation()
{
	logger.trace("BoneAnimation destructor called");
}
