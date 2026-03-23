#include "../../include/ecs/animator_component.h"

#include "../../include/singleton.h"



engine::AnimatorComponent::AnimatorComponent(std::shared_ptr<Animator> animator)
	: m_animator(animator)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(animator));
}

void engine::AnimatorComponent::init(Transform& transform)
{
	m_animator->playAnimationAtIndex(0);
}

void engine::AnimatorComponent::update(float deltaTime, Transform& transform)
{
	m_animator->updateAnimation(deltaTime);
}

void engine::AnimatorComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	m_animator->draw(shader, localTransform);
}

engine::AABB engine::AnimatorComponent::generateBoundingVolume(const std::shared_ptr<Animator> animator)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::AnimatorComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, engine::EditorProperty> engine::AnimatorComponent::getPublicProperties()
{
	return m_animator->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::AnimatorComponent::getPropertySetters()
{
	return m_animator->getPropertySetters();
}

void engine::AnimatorComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
}

void engine::AnimatorComponent::setEnabled(bool enabled)
{
	ComponentBase<AnimatorComponent>::setEnabled(enabled);

	if (!enabled) {
		m_animator->stopAnimation();
	}
	else {
		m_animator->playAnimation();
	}
}