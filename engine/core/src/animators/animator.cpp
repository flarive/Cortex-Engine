#include "../../include/animators/animator.h"

engine::Animator::Animator(std::shared_ptr<Animation> animation)
	: m_CurrentTime(0.0), m_animations{ animation }, m_CurrentAnimation(animation)
{
}

engine::Animator::Animator(std::vector<std::shared_ptr<Animation>>& animations)
	: m_CurrentTime(0.0), m_animations{ animations }, m_CurrentAnimation(animations[0])
{
}