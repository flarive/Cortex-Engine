#pragma once

#include "animator.h"
#include "../animations/transform_animation.h"

namespace engine
{
	class TransformAnimator final : public Animator
	{
	public:
		TransformAnimator(std::shared_ptr<TransformAnimation> animation);
		TransformAnimator(const std::vector<std::shared_ptr<TransformAnimation>>& animations);
		~TransformAnimator() = default;

		virtual AnimatorType getTypeID() const
		{
			return AnimatorType::transform;
		}

		ordered_map<std::string, EditorProperty> getPublicProperties() override {
			return {

			};
		}
		std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
			return {
			};
		}

		void init(Transform& transform) override;
		void update(float dt, Transform& transform) override;
		void draw(Shader& shader, Transform& localTransform) override;

		void playAnimation(std::shared_ptr<Animation> pAnimation) override;
		void playAnimationAtIndex(unsigned short index) override;
		void playAnimation() override;
		void stopAnimation() override;


	private:
		std::shared_ptr<TransformAnimation> m_currentTransformAnimation{};
		Transform m_animatedResult{};   // Temporary storage for each frame
	};
}