#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const AnimTransform& animationTransform)
	: Animation(animationName, nullptr, 1.0f), m_animTransform(animationTransform)
{
}