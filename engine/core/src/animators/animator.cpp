#include "../../include/animators/animator.h"

#include "../../include/managers/log_manager.h"

engine::Animator::Animator(std::shared_ptr<Animation> animation)
	: m_currentTime(0.0), m_animations{ animation }, m_currentAnimation(animation)
{
	logger.trace("Animator base constructor called");
}

engine::Animator::Animator(const std::vector<std::shared_ptr<Animation>>& animations)
	: m_currentTime(0.0), m_animations{ animations }, m_currentAnimation(animations[0])
{
	logger.trace("Animator base constructor called");
}

engine::Animator::~Animator()
{
	logger.trace("Animator base destructor called");
}