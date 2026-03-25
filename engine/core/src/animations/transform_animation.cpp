#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const glm::vec3& rotation, float speedFactor)
	: Animation(animationName, nullptr, speedFactor), m_rotation(rotation)
{
}