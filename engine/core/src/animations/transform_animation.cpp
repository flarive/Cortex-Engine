#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const glm::vec3& rotation, std::shared_ptr<Model> model, float speedFactor)
	: Animation(animationName, rotation, model, speedFactor)
{
	m_rotation = rotation;
}