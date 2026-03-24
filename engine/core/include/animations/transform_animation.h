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
		TransformAnimation(const std::string& animationName, const glm::vec3& rotation, std::shared_ptr<Model> model, float speedFactor = 1.0f);

		~TransformAnimation() = default;

		AnimationType getTypeID() const override
		{
			return AnimationType::transform;
		}

	private:
		glm::vec3 m_rotation{};
	};
}

