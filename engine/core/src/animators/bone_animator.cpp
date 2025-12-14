#include "../../include/animators/bone_animator.h"


engine::BonesAnimator::BonesAnimator(std::shared_ptr<Animation> animation)
	: Animator(animation)
{
	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
	{
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
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

	auto boneInfoMap = m_CurrentAnimation->getBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
	{
		calculateBoneTransform(&node->children[i], globalTransformation);
	}
}