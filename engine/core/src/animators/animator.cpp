#include "../../include/animators/animator.h"

engine::Animator::Animator(std::shared_ptr<BoneAnimation> animation)
	: m_CurrentTime(0.0), m_animations{ animation }, m_currentAnimation(animation)
{
}

engine::Animator::Animator(const std::vector<std::shared_ptr<BoneAnimation>>& animations)
	: m_CurrentTime(0.0), m_animations{ animations }, m_currentAnimation(animations[0])
{
}