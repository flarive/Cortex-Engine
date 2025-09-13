#include "../../include/ecs/primitive_component.h"

engine::PrimitiveComponent::PrimitiveComponent(std::shared_ptr<Primitive> primitive)
    : m_primitive(primitive)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(primitive));
}

void engine::PrimitiveComponent::init()
{

}

void engine::PrimitiveComponent::update(Transform& transform)
{

}

void engine::PrimitiveComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{
	m_primitive->draw(shader, transform);
}

void engine::PrimitiveComponent::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{
	m_primitive->draw(shader, position, size, rotation);
}

engine::AABB engine::PrimitiveComponent::generateBoundingVolume(const std::shared_ptr<Primitive> primitive)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::lowest()); // Use lowest(), not min()

	std::vector<Vertex> vertices = primitive->generateVertices();

	for (const auto& vertex : vertices)
	{
		minAABB.x = std::min(minAABB.x, vertex.position.x);
		minAABB.y = std::min(minAABB.y, vertex.position.y);
		minAABB.z = std::min(minAABB.z, vertex.position.z);

		maxAABB.x = std::max(maxAABB.x, vertex.position.x);
		maxAABB.y = std::max(maxAABB.y, vertex.position.y);
		maxAABB.z = std::max(maxAABB.z, vertex.position.z);
	}

	return engine::AABB(minAABB, maxAABB);
}

std::unique_ptr<engine::AABB> engine::PrimitiveComponent::getBoundingVolume()
{
	return std::move(m_boundingVolume);
}