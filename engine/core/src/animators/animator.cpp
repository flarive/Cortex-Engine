#include "../../include/animators/animator.h"

engine::Animator::Animator(std::shared_ptr<Animation> animation)
	: m_CurrentTime(0.0), m_CurrentAnimation(animation)
{

}
