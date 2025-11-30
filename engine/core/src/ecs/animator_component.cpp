#include "../../include/ecs/animator_component.h"

#include "../../include/singleton.h"



engine::AnimatorComponent::AnimatorComponent(std::shared_ptr<Animator> animator)
	: m_animator(animator)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(animator));
}

void engine::AnimatorComponent::init(Transform& transform)
{

}

void engine::AnimatorComponent::update(float deltaTime, Transform& transform)
{
	m_animator->updateAnimation(deltaTime);
}

void engine::AnimatorComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	auto transforms = m_animator->getFinalBoneMatrices();

	shader.setBool("isAnimated", false);

	for (int i = 0; i < transforms.size(); ++i)
	{
		shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
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

engine::ordered_map<std::string, std::variant<int, std::string, float, bool>> engine::AnimatorComponent::getPublicProperties()
{
	return engine::ordered_map<std::string, std::variant<int, std::string, float, bool>>{};
}

std::unordered_map<std::string, std::function<void(float)>> engine::AnimatorComponent::getPropertySetters()
{
	return std::unordered_map<std::string, std::function<void(float)>>();
}



