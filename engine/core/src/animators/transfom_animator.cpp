#include "../../include/animators/transform_animator.h"


engine::TransformAnimator::TransformAnimator(const glm::vec3& rotation)
	: Animator(nullptr), m_rotation(rotation)
{
}

void engine::TransformAnimator::update(float dt)
{
	m_DeltaTime = dt;
	if (m_isPlaying)
	{
		//m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		//m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());

		m_internalRotation += dt * 0.002f;
	}
}

void engine::TransformAnimator::draw(Shader& shader, Transform& localTransform)
{
	auto rot = localTransform.getLocalRotation();
	localTransform.setLocalRotation(glm::vec3(rot.x, rot.y + m_internalRotation, rot.z));
}

void engine::TransformAnimator::playAnimation(std::shared_ptr<BoneAnimation> pAnimation)
{
	//m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
	m_isPlaying = true;
}

void engine::TransformAnimator::playAnimation()
{
	//if (m_CurrentAnimation)
		m_isPlaying = true;
	/*else
		m_isPlaying = false;*/
}

void engine::TransformAnimator::stopAnimation()
{
	m_isPlaying = false;
}

void engine::TransformAnimator::playAnimationAtIndex(unsigned short index)
{
	//unsigned short loop = 0;
	//for (const auto& animation : m_animations)
	//{
	//	if (loop == index)
	//	{
	//		playAnimation(animation);
	//		break;
	//	}

	//	loop++;
	//}
}