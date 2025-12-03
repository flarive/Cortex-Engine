#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/importer.hpp>

#include "animation.h"
#include "bone.h"

namespace engine
{
	class Animator
	{
	public:
		Animator(std::shared_ptr<Animation> animation) : m_CurrentTime(0.0), m_CurrentAnimation(animation)
		{
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
			{
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
			}
		}

		void updateAnimation(float dt)
		{
			m_DeltaTime = dt;
			if (m_CurrentAnimation)
			{
				m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
				m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
				calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
			}
		}

		void playAnimation(std::shared_ptr<Animation> pAnimation)
		{
			m_CurrentAnimation = pAnimation;
			m_CurrentTime = 0.0f;
		}

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
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

		std::vector<glm::mat4> getFinalBoneMatrices()
		{
			return m_FinalBoneMatrices;
		}

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices{};
		std::shared_ptr<Animation> m_CurrentAnimation{};
		float m_CurrentTime{};
		float m_DeltaTime{};
	};
}