#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <functional>

#include "animator.h"
#include "../animations/bone_animation.h"
#include "../models/bone.h"

namespace engine
{
	class BonesAnimator final : public Animator
	{
	public:
		BonesAnimator(std::shared_ptr<BoneAnimation> animation);
		BonesAnimator(const std::vector<std::shared_ptr<BoneAnimation>>& animations);
		~BonesAnimator() override;

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

		std::shared_ptr<BoneAnimation>& getCurrentAnimation() { return m_currentBoneAnimation; }
		std::vector<std::shared_ptr<Animation>>& getAnimations() { return m_animations; }
		std::vector<std::string> getAnimationsStringList();
		
		void init(Transform& transform) override;
		void update(float dt, Transform& transform) override;
		void draw(Shader& shader, Transform& localTransform) override;

		void playAnimation(std::shared_ptr<Animation> pAnimation) override;
		void playAnimationAtIndex(unsigned short index) override;
		void playAnimation() override;
		void stopAnimation() override;
		

	private:
		std::shared_ptr<BoneAnimation> m_currentBoneAnimation{};
		
		std::map<std::string, std::vector<glm::mat4>> m_animationsFinalBoneMatrices{};
		void calculateBoneTransform(const AnimNodeData* node, glm::mat4 parentTransform);
		const std::vector<glm::mat4>& getFinalBoneMatrices() const;
	};
}