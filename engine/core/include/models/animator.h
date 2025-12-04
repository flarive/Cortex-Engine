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
		Animator(std::shared_ptr<Animation> animation);
		~Animator() = default;

		void updateAnimation(float dt);
		void playAnimation(std::shared_ptr<Animation> pAnimation);
		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	

		std::vector<glm::mat4> getFinalBoneMatrices() { return m_FinalBoneMatrices;	}

	private:
		std::vector<glm::mat4> m_FinalBoneMatrices{};
		std::shared_ptr<Animation> m_CurrentAnimation{};
		float m_CurrentTime{};
		float m_DeltaTime{};
	};
}