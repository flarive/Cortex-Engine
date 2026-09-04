#include "../../include/animations/bone_animation.h"

#include "../../include/managers/log_manager.h"


engine::BoneAnimation::BoneAnimation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor)
	: Animation(animationName, model, speedFactor), m_filepath(animationPath)
{
	logger.trace("BoneAnimation constructor called");

	// Create the right animation loader according file extension (tinyGLTF for GLTF otherwise Assimp)
	m_animationLoader = AnimationLoader::create(animationPath);
	m_animationLoader->loadAnimation(animationPath, model);


	m_duration = m_animationLoader->getDuration();
	m_durationInSeconds = m_animationLoader->getDurationInSeconds();
	m_ticksPerSecond = m_animationLoader->getTicksPerSecond();
	m_desiredFPS = m_animationLoader->getDesiredFPS();
	m_numFrames = m_animationLoader->getNumFrames();
}

const std::vector<engine::Bone>& engine::BoneAnimation::getBones() const
{
	if (m_animationLoader)
		return m_animationLoader->getBones();

	static std::vector<engine::Bone> emptyBones;
	return emptyBones;
}

const size_t engine::BoneAnimation::getBoneCount() const
{
	if (m_animationLoader)
		return m_animationLoader->getBoneCount();

	return 0;
}

const engine::AnimNodeData& engine::BoneAnimation::getRootNode() const
{
	if (m_animationLoader)
		return m_animationLoader->getRootNode();

	static AnimNodeData emptyAnimNodeData;
	return emptyAnimNodeData;
}

const std::map<std::string, engine::BoneInfo>& engine::BoneAnimation::getBonesInfoMap() const
{
	if (m_animationLoader)
		return m_animationLoader->getBonesInfoMap();

	static std::map<std::string, engine::BoneInfo> emptyBonesInfoMap;
	return emptyBonesInfoMap;
}

engine::Bone* engine::BoneAnimation::findBone(const std::string& name)
{
	return m_animationLoader->findBone(name);
}

engine::BoneAnimation::~BoneAnimation()
{
	logger.trace("BoneAnimation destructor called");
}
