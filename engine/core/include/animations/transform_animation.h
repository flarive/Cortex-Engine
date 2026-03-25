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
		TransformAnimation(const std::string& animationName, const glm::vec3& rotation, float speedFactor = 1.0f);

		~TransformAnimation() = default;

		AnimationType getTypeID() const override
		{
			return AnimationType::transform;
		}

		glm::vec3& getRotation() { return m_rotation; }
		float& getInternalRotation() { return m_internalRotation; }

	private:
		glm::vec3 m_rotation{};
		float m_internalRotation{};
	};
}