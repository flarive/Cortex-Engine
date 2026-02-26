#include "../../include/animators/bone_animator.h"

engine::BonesAnimator::BonesAnimator(std::shared_ptr<Animation> animation)
	: Animator(animation)
{
	m_animationsFinalBoneMatrices.clear();

	m_boneCount = animation->getBoneCount();
	if (m_boneCount > 0)
	{
		auto animmFinalBoneMatrices = std::vector<glm::mat4>();
		animmFinalBoneMatrices.reserve(m_boneCount);

		for (unsigned int i = 0; i < m_boneCount; i++)
		{
			animmFinalBoneMatrices.push_back(glm::mat4(1.0f));
		}

		// create a new entry
		m_animationsFinalBoneMatrices.emplace(animation->getName(), animmFinalBoneMatrices);
	}
}

engine::BonesAnimator::BonesAnimator(std::vector<std::shared_ptr<Animation>>& animations)
	: Animator(animations)
{
	m_animationsFinalBoneMatrices.clear();

	if (animations.size() > 0)
	{
		m_boneCount = animations[0]->getBoneCount();
		if (m_boneCount > 0)
		{
			for (const auto& animation : m_animations)
			{
				auto animmFinalBoneMatrices = std::vector<glm::mat4>();
				animmFinalBoneMatrices.reserve(m_boneCount);

				for (unsigned int i = 0; i < m_boneCount; i++)
				{
					animmFinalBoneMatrices.push_back(glm::mat4(1.0f));
				}

				// create a new entry
				m_animationsFinalBoneMatrices.emplace(animation->getName(), animmFinalBoneMatrices);
			}
		}
	}
}

std::vector<std::string> engine::BonesAnimator::getAnimationsStringList()
{
	std::vector<std::string> names;
	names.reserve(m_animations.size()); // avoids reallocation growth

	for (const auto& animation : m_animations) {
		names.emplace_back(animation->getName()); // copies/moves the returned string
	}

	return names; // NRVO/move elision
}

void engine::BonesAnimator::updateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation && m_isPlaying)
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
	m_isPlaying = true;
}

void engine::BonesAnimator::playAnimation()
{
	if (m_CurrentAnimation)
		m_isPlaying = true;
	else
		m_isPlaying = false;
}

void engine::BonesAnimator::stopAnimation()
{
	m_isPlaying = false;
}

void engine::BonesAnimator::playAnimationAtIndex(unsigned short index)
{
	unsigned short loop = 0;
	for (const auto& animation : m_animations)
	{
		if (loop == index)
		{
			playAnimation(animation);
			break;
		}

		loop++;
	}
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
	const std::string& nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = m_CurrentAnimation->findBone(nodeName);

	if (bone)
	{
		bone->update(m_CurrentTime);
		nodeTransform = bone->getLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	const std::string& currentAnimName = m_CurrentAnimation->getName();

	bool keyExists = m_animationsFinalBoneMatrices.find(currentAnimName) != m_animationsFinalBoneMatrices.end();

	assert(keyExists && "Key not found !");

	if (keyExists)
	{
		auto boneInfoMap = m_CurrentAnimation->getBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id; // &&&&&&&&&&&&& ???????????????
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_animationsFinalBoneMatrices[currentAnimName][index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
		{
			calculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
}