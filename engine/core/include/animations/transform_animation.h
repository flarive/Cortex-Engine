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
		TransformAnimation(const std::string& animationName, const AnimTransform& animationTransform);
		~TransformAnimation() override;

		AnimationType getTypeID() const override
		{
			return AnimationType::transform;
		}

		AnimTransform& getAnimTransform() { return m_animTransform; }

	private:
		AnimTransform m_animTransform{};
	};
}