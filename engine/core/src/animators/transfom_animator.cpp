#include "../../include/animators/transform_animator.h"


engine::TransformAnimator::TransformAnimator(std::shared_ptr<TransformAnimation> animation)
	: Animator(std::static_pointer_cast<Animation>(animation))
{
	m_currentTransformAnimation = std::static_pointer_cast<TransformAnimation>(m_currentAnimation);
}

engine::TransformAnimator::TransformAnimator(const std::vector<std::shared_ptr<TransformAnimation>>& animations)
	: Animator(std::vector<std::shared_ptr<Animation>>(animations.begin(), animations.end()))
{
	m_currentTransformAnimation = std::static_pointer_cast<TransformAnimation>(m_currentAnimation);
}

void engine::TransformAnimator::update(float dt)
{
	m_deltaTime = dt;
	if (m_isPlaying)
	{
		//m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		//m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());

		//m_internalRotation += dt * 0.002f;
		
		m_currentTransformAnimation->getInternalRotation() += dt * 0.002f;
	}
}

void engine::TransformAnimator::draw(Shader& shader, Transform& localTransform)
{
	auto rot = localTransform.getLocalRotation();
	localTransform.setLocalRotation(glm::vec3(rot.x, rot.y + m_currentTransformAnimation->getInternalRotation(), rot.z));
}

void engine::TransformAnimator::playAnimation(std::shared_ptr<Animation> pAnimation)
{
	m_currentAnimation = pAnimation;
	m_currentTime = 0.0f;
	m_isPlaying = true;
}

void engine::TransformAnimator::playAnimation()
{
	if (m_currentAnimation)
		m_isPlaying = true;
	else
		m_isPlaying = false;
}

void engine::TransformAnimator::stopAnimation()
{
	m_isPlaying = false;
}

void engine::TransformAnimator::playAnimationAtIndex(unsigned short index)
{
	unsigned short loop = 0;
	for (const auto& animation : m_animations)
	{
		if (loop == index)
		{
			playAnimation(animation);
			break;
		}

		loop++;
	}
}