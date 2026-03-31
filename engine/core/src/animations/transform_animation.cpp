#include "../../include/animations/transform_animation.h"

engine::TransformAnimation::TransformAnimation(const std::string & animationName, const AnimTransform& animationTransform)
	: Animation(animationName, nullptr, 1.0f), m_animTransform(animationTransform)
{

    m_durationInSeconds = m_animTransform.durationSeconds;

    m_ticksPerSecond = 1000.0;                // use millisecond tick convention
    m_duration = m_durationInSeconds * m_ticksPerSecond;

    m_desiredFPS = 60; // or 30, or any FPS you decide for TRS animations
    m_numFrames = m_durationInSeconds * m_desiredFPS;
}