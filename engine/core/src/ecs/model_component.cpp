#include "../../include/ecs/model_component.h"



engine::ModelComponent::ModelComponent(std::shared_ptr<Model> model)
    : m_model(model)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(model));
}


void engine::ModelComponent::init(Transform& transform)
{
	m_model->position = transform.getLocalPosition();
	m_model->rotation = transform.getLocalRotation();
	m_model->scale = transform.getLocalScale();
}

void engine::ModelComponent::update(Transform& transform)
{

}

void engine::ModelComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform)
{
    m_model->draw(shader, worldTransformMatrix, localTransform);
}

engine::AABB engine::ModelComponent::generateBoundingVolume(const std::shared_ptr<Model> model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	if (model)
	{
		for (auto&& mesh : model->meshes)
		{
			for (auto&& vertex : mesh.vertices)
			{
				minAABB.x = std::min(minAABB.x, vertex.position.x);
				minAABB.y = std::min(minAABB.y, vertex.position.y);
				minAABB.z = std::min(minAABB.z, vertex.position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.position.z);
			}
		}
	}
	return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::ModelComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}
