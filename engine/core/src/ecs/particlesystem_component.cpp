#include "../../include/ecs/particlesystem_component.h"

#include "../../include/singleton.h"

#include "../../include/primitives/primitive.h"


engine::ParticleSystemComponent::ParticleSystemComponent(std::shared_ptr<ParticleSystem> particleSystem) : m_particleSystem(particleSystem)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(particleSystem));

	//auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	//m_debug_boundingBox = std::make_unique<DebugCube>(width, height, depth); // Cube at origin with dimensions of the AABB
	//m_debug_boundingBox->setup();


	// Initialize property setters based on primitive type
	m_propertySetters = m_particleSystem->getPropertySetters();
}

void engine::ParticleSystemComponent::init(Transform& transform)
{
	//m_particleSystem->setPosition(transform.getLocalPosition());
	//m_particleSystem->setRotation(transform.getLocalRotation());
	//m_particleSystem->setScale(transform.getLocalScale());
}

void engine::ParticleSystemComponent::update(float deltaTime, Transform& transform)
{

}

void engine::ParticleSystemComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	//m_particleSystem->draw(shader, projection, view, worldTransformMatrix, localTransform);

	//auto* singleton = engine::Singleton::getInstance();
	//assert(singleton != nullptr && "Singleton not initialized !");
	//SceneSettings& sceneSettings = singleton->sceneSettings();

	//if (sceneSettings.drawBoundingBoxesVisualHelpers)
	//	m_debug_boundingBox->draw(projection, view, worldTransformMatrix, localTransform);
}

engine::AABB engine::ParticleSystemComponent::generateBoundingVolume(const std::shared_ptr<ParticleSystem> primitive)
{
	//glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	//glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::lowest()); // Use lowest(), not min()

	//std::vector<Vertex> vertices = primitive->generateVertices();

	//for (const auto& vertex : vertices)
	//{
	//	minAABB.x = std::min(minAABB.x, vertex.position.x);
	//	minAABB.y = std::min(minAABB.y, vertex.position.y);
	//	minAABB.z = std::min(minAABB.z, vertex.position.z);

	//	maxAABB.x = std::max(maxAABB.x, vertex.position.x);
	//	maxAABB.y = std::max(maxAABB.y, vertex.position.y);
	//	maxAABB.z = std::max(maxAABB.z, vertex.position.z);
	//}

	//return engine::AABB(minAABB, maxAABB);

	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::ParticleSystemComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, engine::EditorProperty> engine::ParticleSystemComponent::getPublicProperties()
{
	return m_particleSystem->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::ParticleSystemComponent::getPropertySetters()
{
	return m_particleSystem->getPropertySetters();
}

void engine::ParticleSystemComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
	auto it = m_propertySetters.find(key);
	if (it != m_propertySetters.end())
	{
		it->second(value);
		m_particleSystem->reSetup(); // Assuming all primitives have a reSetup() method
	}
}

void engine::ParticleSystemComponent::setEnabled(bool enabled)
{
	ComponentBase<ParticleSystemComponent>::setEnabled(enabled);

	m_particleSystem->setEnabled(enabled);
}
