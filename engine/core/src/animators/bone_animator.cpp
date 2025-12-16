#include "../../include/animators/bone_animator.h"

engine::BonesAnimator::BonesAnimator(std::shared_ptr<Animation> animation)
	: Animator(animation)
{
	m_animationsFinalBoneMatrices.clear();
	
	auto animmFinalBoneMatrices = std::vector<glm::mat4>();
	animmFinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
	{
		animmFinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	// create a new entry
	m_animationsFinalBoneMatrices.emplace(animation->getName(), animmFinalBoneMatrices);
}

engine::BonesAnimator::BonesAnimator(std::vector<std::shared_ptr<Animation>>& animations)
	: Animator(animations)
{
	m_animationsFinalBoneMatrices.clear();

	for (auto it = animations.begin(); it != animations.end(); ++it)
	{
		auto animation = *it;

		auto animmFinalBoneMatrices = std::vector<glm::mat4>();
		animmFinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
		{
			animmFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		// create a new entry
		m_animationsFinalBoneMatrices.emplace(animation->getName(), animmFinalBoneMatrices);
	}
}

void engine::BonesAnimator::updateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
		calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void engine::BonesAnimator::playAnimation(std::shared_ptr<Animation> pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

const std::vector<glm::mat4>& engine::BonesAnimator::getFinalBoneMatrices() const
{
	static const std::vector<glm::mat4> kEmpty; // lives forever

	std::string currentAnimName = m_CurrentAnimation->getName();

	bool keyExists = m_animationsFinalBoneMatrices.find(currentAnimName) != m_animationsFinalBoneMatrices.end();

	assert(keyExists && "Key not found !");

	if (keyExists)
	{
		return m_animationsFinalBoneMatrices.at(currentAnimName);
	}

	return kEmpty;
}

void engine::BonesAnimator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->findBone(nodeName);

	if (Bone)
	{
		Bone->update(m_CurrentTime);
		nodeTransform = Bone->getLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	std::string currentAnimName = m_CurrentAnimation->getName();

	bool keyExists = m_animationsFinalBoneMatrices.find(currentAnimName) != m_animationsFinalBoneMatrices.end();

	assert(keyExists && "Key not found !");

	if (keyExists)
	{
		auto boneInfoMap = m_CurrentAnimation->getBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_animationsFinalBoneMatrices[currentAnimName][index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
}