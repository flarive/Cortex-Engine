#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/importer.hpp>

#include "animator.h"
#include "animation.h"
#include "../models/bone.h"

namespace engine
{

	class BonesAnimator final : public Animator
	{
	public:
		BonesAnimator(std::shared_ptr<Animation> animation);
		BonesAnimator(std::vector<std::shared_ptr<Animation>>& animations);
		~BonesAnimator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::bones;
		}

		ordered_map<std::string, EditorProperty> getPublicProperties() override {
			return {
				{"animation_name", EditorProperty { "Animation name", getCurrentAnimation()->getName(), 0.0f, 0.0f, 0.0f, "", true}},
				{"animation_file", EditorProperty { "Animation path", getCurrentAnimation()->getFilepath(), 0.0f, 0.0f, 0.0f, "", true}},
				{"animation_duration", EditorProperty { "Animation duration", getCurrentAnimation()->getDurationInSeconds(), 0.0f, 100.0f, 1.0f, "%.2f", true}},
				{"animation_frames", EditorProperty { "Animation frames", getCurrentAnimation()->getFramesCount(), 0.0f, 100.0f, 1.0f, "%.2f", true}}
			};
		}
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
			return {
			};
		}

		std::shared_ptr<Animation>& getCurrentAnimation() { return m_CurrentAnimation; }

		void updateAnimation(float dt) override;
		void playAnimation(std::shared_ptr<Animation> pAnimation) override;

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	

		std::vector<glm::mat4> getFinalBoneMatrices() override;

	private:
		//std::vector<glm::mat4> m_FinalBoneMatrices{}; // old

		std::map < std::string, std::vector<glm::mat4>> m_animationsFinalBoneMatrices{};
	};
}