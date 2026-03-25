#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const AnimTransform& animationTransform, float duration)
	: Animation(animationName, nullptr, 1.0f), m_animTransform(animationTransform), m_duration(duration)
{
}