#include "../../include/animators/transform_animator.h"


engine::TransformAnimator::TransformAnimator(std::shared_ptr<TransformAnimation> animation)
	: Animator(std::static_pointer_cast<Animation>(animation)), m_currentTransformAnimation(std::static_pointer_cast<TransformAnimation>(m_currentAnimation))
{
}

engine::TransformAnimator::TransformAnimator(const std::vector<std::shared_ptr<TransformAnimation>>& animations)
	: Animator(std::vector<std::shared_ptr<Animation>>(animations.begin(), animations.end())), m_currentTransformAnimation(std::static_pointer_cast<TransformAnimation>(m_currentAnimation))
{
}

void engine::TransformAnimator::init(Transform& transform)
{
	m_initialTransform = transform;
}

void engine::TransformAnimator::update(float dt, Transform& transform)
{
	m_deltaTime = dt;
	if (m_isPlaying && m_currentTransformAnimation)
	{
		//m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
		//m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());


		auto& animTransform = m_currentTransformAnimation->getAnimTransform();
		bool finished = animTransform.update(dt, m_animatedResult);
		if (finished)
		{
			m_isPlaying = false;  // or loop, depending on your system
		}
	}
}

void engine::TransformAnimator::draw(Shader& shader, Transform& localTransform)
{
	// Officially updates the entity's transform in the world
	if (m_isPlaying)
	{
		localTransform = m_animatedResult;
	}
}

//void engine::TransformAnimator::playAnimation(std::shared_ptr<Animation> pAnimation)
//{
//	// force current animation to be the one we want to play
//	m_currentAnimation = pAnimation;
//	m_currentTransformAnimation = std::static_pointer_cast<TransformAnimation>(m_currentAnimation);
//
//	m_currentTime = 0.0f;
//	m_isPlaying = true;
//
//	//if (m_currentTransformAnimation)
//	//{
//	//	auto& animTransform = m_currentTransformAnimation->getAnimTransform();
//	//	
//	//	// Setup the TRS animation
//	//	m_animTransform.duration = 5.0f;
//	//	m_animTransform.setup(
//	//		m_currentTransformAnimation->currentTransform,       // entity's current TRS
//	//		m_currentTransformAnimation->from,
//	//		m_currentTransformAnimation->to,
//	//		m_currentTransformAnimation->mode
//	//	);
//	//}
//	
//}

void engine::TransformAnimator::playAnimation(std::shared_ptr<Animation> pAnimation)
{
	m_currentAnimation = pAnimation;
	m_currentTransformAnimation = std::static_pointer_cast<TransformAnimation>(m_currentAnimation);

	m_currentTime = 0.0f;
	m_isPlaying = true;

	if (m_currentTransformAnimation)
	{
		AnimTransform& anim = m_currentTransformAnimation->getAnimTransform();

		// Reset elapsed every time
		anim.elapsed = 0.0f;

		// Make sure duration is valid
		if (anim.duration <= 0.0f)
			anim.duration = 1.0f;

		// IMPORTANT: re-setup using the entity’s actual transform
		anim.setup(
			m_initialTransform,
			anim.from,
			anim.to,
			anim.mode
		);
	}
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