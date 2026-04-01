#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const AnimTransform& animationTransform)
	: Animation(animationName, nullptr, 1.0f), m_animTransform(animationTransform)
{
    m_durationInSeconds = m_animTransform.duration;

    m_ticksPerSecond = 1000; // use millisecond tick convention
    m_duration = m_durationInSeconds * m_ticksPerSecond;

    m_desiredFPS = 30; // or 30, or any FPS you decide for TRS animations
    m_numFrames = static_cast<unsigned int>(m_durationInSeconds * m_desiredFPS);
}