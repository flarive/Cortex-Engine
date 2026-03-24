#include "../../include/animators/animator.h"

engine::Animator::Animator(std::shared_ptr<BoneAnimation> animation)
	: m_CurrentTime(0.0), m_animations{ animation }, m_CurrentAnimation(animation)
{
}

engine::Animator::Animator(std::vector<std::shared_ptr<BoneAnimation>>& animations)
	: m_CurrentTime(0.0), m_animations{ animations }, m_CurrentAnimation(animations[0])
{
}