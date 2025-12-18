#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <functional>
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
				{"animation_name", EditorProperty { "Current anim", getCurrentAnimation()->getName(), true}},
				{"animation_file", EditorProperty { "Animation path", getCurrentAnimation()->getFilepath(), true}},
				{"animation_duration", EditorProperty { "Animation duration", getCurrentAnimation()->getDurationInSeconds(), true}},
				{"animation_frames", EditorProperty { "Animation frames", getCurrentAnimation()->getFramesCount(), true}},
				{"animations", EditorProperty { "Animations",	getAnimationsStringList(), true, 0.0f, 0.0f, 0.0f, "", [this](unsigned short index) { this->playAnimationAtIndex(index); } }}
			};
		}
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
			return {
			};
		}

		std::shared_ptr<Animation>& getCurrentAnimation() { return m_CurrentAnimation; }
		std::vector<std::shared_ptr<Animation>>& getAnimations() { return m_animations; }
		std::vector<std::string> getAnimationsStringList();
		

		void updateAnimation(float dt) override;
		void playAnimation(std::shared_ptr<Animation> pAnimation) override;

		
		void playAnimationAtIndex(unsigned short index);
		
	

		const std::vector<glm::mat4>& getFinalBoneMatrices() const override;

	private:
		std::map<std::string, std::vector<glm::mat4>> m_animationsFinalBoneMatrices{};

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	};
}