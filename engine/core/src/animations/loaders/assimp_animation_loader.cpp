#include "../../../include/animations/loaders/assimp_animation_loader.h"

#include "../../../include/managers/log_manager.h"

#include <assimp/importer.hpp>
#include <assimp/postprocess.h>


engine::AssimpAnimationLoader::AssimpAnimationLoader()
{
	logger.trace("AssimpAnimationLoader constructor called");
}

void engine::AssimpAnimationLoader::loadAnimation(const std::string& animationPath, std::shared_ptr<Model> model)
{
	importBoneAnimation(animationPath, model);
}

void engine::AssimpAnimationLoader::importBoneAnimation(const std::string& animationPath, std::shared_ptr<Model> model)
{
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

	// Build hierarchy from first root node
	readHierarchyData(m_rootNode, scene->mRootNode);

	// Bones
	readMissingBones(animation, *model.get());
}

void engine::AssimpAnimationLoader::readMissingBones(const aiAnimation* animation, Model& model)
{
	auto& boneInfoMap = model.getBoneInfoMap();
	int boneCount = model.getBoneCount();

	for (unsigned i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
			boneInfoMap[boneName].id = boneCount++;

		std::vector<KeyPosition> positions;
		std::vector<KeyRotation> rotations;
		std::vector<KeyScale> scales;

		extractBoneKeys(channel, positions, rotations, scales);

		Bone bone = createBone(boneName, boneInfoMap[boneName].id, positions, rotations, scales);
		m_bones.push_back(bone);
	}

	m_boneInfoMap = boneInfoMap;
}

void engine::AssimpAnimationLoader::extractBoneKeys(const aiNodeAnim* channel, std::vector<KeyPosition>& positions, std::vector<KeyRotation>& rotations, std::vector<KeyScale>& scales)
{
	for (unsigned i = 0; i < channel->mNumPositionKeys; ++i)
		positions.push_back({
			AssimpGLMHelpers::GetGLMVec(channel->mPositionKeys[i].mValue),
			float(channel->mPositionKeys[i].mTime)
			});

	for (unsigned i = 0; i < channel->mNumRotationKeys; ++i)
		rotations.push_back({
			AssimpGLMHelpers::GetGLMQuat(channel->mRotationKeys[i].mValue),
			float(channel->mRotationKeys[i].mTime)
			});

	for (unsigned i = 0; i < channel->mNumScalingKeys; ++i)
		scales.push_back({
			AssimpGLMHelpers::GetGLMVec(channel->mScalingKeys[i].mValue),
			float(channel->mScalingKeys[i].mTime)
			});
}

void engine::AssimpAnimationLoader::readHierarchyData(AnimNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AnimNodeData newData;
		readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

// ensure anim was exported at 30 FPS (should be always the case for mixamo anims)
// should be 0, 33.3333, 66.6667, 100...
// 33.33 ms = 1/30s
unsigned int engine::AssimpAnimationLoader::computeFPS(const aiAnimation* anim)
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

engine::AssimpAnimationLoader::~AssimpAnimationLoader()
{
	logger.trace("AssimpAnimationLoader destructor called");
}
