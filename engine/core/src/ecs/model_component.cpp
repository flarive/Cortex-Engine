#include "../../include/ecs/model_component.h"

#include "../../include/singleton.h"

engine::ModelComponent::ModelComponent(std::shared_ptr<Model> model)
    : m_model(model)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(model));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<DebugCube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup();

	// Initialize property setters based on primitive type
	m_propertySetters = m_model->getPropertySetters();
}

void engine::ModelComponent::init(Transform& transform)
{
	m_model->setTransform(transform.getLocalPosition(), transform.getLocalRotation(), transform.getLocalScale());
}

void engine::ModelComponent::update(float deltaTime, Transform& transform)
{
	
}

void engine::ModelComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
    m_model->draw(shader, worldTransformMatrix, localTransform);

	auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
	SceneSettings& sceneSettings = singleton->sceneSettings();

	if (sceneSettings.drawBoundingBoxesVisualHelpers)
		m_debug_boundingBox->draw(projection, view, worldTransformMatrix, localTransform);
}

engine::AABB engine::ModelComponent::generateBoundingVolume(const std::shared_ptr<Model> model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	if (model)
	{
		for (auto&& mesh : model->getMeshes())
		{
			for (auto&& vertex : mesh->getVertices())
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

engine::ordered_map<std::string, engine::EditorProperty> engine::ModelComponent::getPublicProperties()
{
	return m_model->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::ModelComponent::getPropertySetters()
{
	return m_model->getPropertySetters();
}

void engine::ModelComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
	auto it = m_propertySetters.find(key);
	if (it != m_propertySetters.end())
	{
		it->second(value);
		m_model->reSetup(); // Assuming all primitives have a reSetup() method
	}
}

void engine::ModelComponent::setEnabled(bool enabled)
{
	ComponentBase<ModelComponent>::setEnabled(enabled);

	m_model->setEnabled(enabled);
}
