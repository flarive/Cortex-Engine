#pragma once

#include "animator.h"
#include "../animations/bone_animation.h"

namespace engine
{
	class RotationAnimator final : public Animator
	{
	public:
		RotationAnimator(const glm::vec3& rotation);
		~RotationAnimator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::rotation;
		}

		ordered_map<std::string, EditorProperty> getPublicProperties() override {
			return {

			};
		}
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
			return {
			};
		}

		void updateAnimation(float dt) override;
		void draw(Shader& shader, Transform& localTransform) override;

		void playAnimation(std::shared_ptr<BoneAnimation> pAnimation) override;
		void playAnimation() override;
		void stopAnimation() override;

		void playAnimationAtIndex(unsigned short index) override;



	private:
		glm::vec3 m_rotation{};
		float m_internalRotation{};
	};
}