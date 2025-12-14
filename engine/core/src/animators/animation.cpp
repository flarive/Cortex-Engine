#include "../../include/animators/animation.h"


engine::Animation::Animation(const std::string& animationPath, std::shared_ptr<Model> model, float speed)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	aiAnimation* animation = scene->mAnimations[0];
	m_duration = static_cast<float>(animation->mDuration); //ms
	m_ticksPerSecond = static_cast<int>(animation->mTicksPerSecond * speed);
	//m_numFrames = static_cast<unsigned int>(animation->mDuration * animation->mTicksPerSecond); // 283 ?

	m_desiredFPS = 30.0f;
	m_durationInSeconds = animation->mDuration / animation->mTicksPerSecond;
	m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);

	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	readHierarchyData(m_rootNode, scene->mRootNode);
	readMissingBones(animation, *model.get());
}

engine::Bone* engine::Animation::findBone(const std::string& name)
{
	auto iter = std::find_if(m_bones.begin(), m_bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.getBoneName() == name;
		}
	);
	if (iter == m_bones.end()) return nullptr;
	else return &(*iter);
}

void engine::Animation::readMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.getBoneInfoMap(); //getting m_boneInfoMap from Model class
	int& boneCount = model.getBoneCount(); //getting the m_boneCounter from Model class

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

void engine::Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
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