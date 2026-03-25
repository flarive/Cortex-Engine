#pragma once

#include <vector>
#include <map>
#include <functional>

#include "animation.h"
#include "../models/model.h"

namespace engine
{
	class TransformAnimation : public Animation
	{
	public:
		TransformAnimation() = default;
		TransformAnimation(const std::string& animationName, const AnimTransform& animationTransform, float duration = 1.0f);
		~TransformAnimation() = default;

		AnimationType getTypeID() const override
		{
			return AnimationType::transform;
		}

		AnimTransform& getAnimTransform() { return m_animTransform; }
		float& getInternalRotation() { return m_internalRotation; }

	private:
		AnimTransform m_animTransform{};
		float m_duration{}; // duration in frames
		float m_internalRotation{};
	};
}