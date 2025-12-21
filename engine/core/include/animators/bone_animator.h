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
				{"label_current_animation", EditorProperty { "Animation", 0, label }},
				{"animation_name", EditorProperty { "Current", getCurrentAnimation()->getName(), readonly }},
				{"animation_file", EditorProperty { "Path", getCurrentAnimation()->getFilepath(), readonly }},
				{"animation_duration", EditorProperty { "Duration", getCurrentAnimation()->getDurationInSeconds(), readonly, 0.0f, 0.0f, 0.0f, "", "sec" }},
				{"animation_frames", EditorProperty { "Length", getCurrentAnimation()->getFramesCount(), readonly, 0.0f, 0.0f, 0.0f, "", "frames" }},
				{"animation_fps", EditorProperty { "FPS", getCurrentAnimation()->getFPS(), readonly }},
				{"animation_speed", EditorProperty { "Speed", getCurrentAnimation()->getSpeedFactor(), readonly }},
				{"label_animations", EditorProperty { "Animations", 0, label }},
				{"animations", EditorProperty { "*", getAnimationsStringList(), noheader | readonly, 0.0f, 0.0f, 0.0f, "", "", [this](unsigned short index) { this->playAnimationAtIndex(index); }}}
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