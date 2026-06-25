#include "../../include/ecs/terrain_component.h"

#include "../../include/singleton.h"

#include "../../include/primitives/terrain.h"


engine::TerrainComponent::TerrainComponent(std::shared_ptr<Terrain> terrain) : m_terrain(terrain)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(terrain));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<DebugCube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup();


	// Initialize property setters based on primitive type
	m_propertySetters = m_terrain->getPropertySetters();
}

void engine::TerrainComponent::init(Transform& transform)
{
	m_terrain->setPosition(transform.getLocalPosition());
	m_terrain->setRotation(transform.getLocalRotation());
	m_terrain->setScale(transform.getLocalScale());
}

void engine::TerrainComponent::update(float deltaTime, Transform& transform)
{

}

void engine::TerrainComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	m_terrain->draw(shader, projection, view, worldTransformMatrix, localTransform);

	auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
	SceneSettings& sceneSettings = singleton->sceneSettings();

	if (sceneSettings.drawBoundingBoxesVisualHelpers)
		m_debug_boundingBox->draw(projection, view, worldTransformMatrix, localTransform);
}

engine::AABB engine::TerrainComponent::generateBoundingVolume(const std::shared_ptr<Terrain> terrain)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::lowest()); // Use lowest(), not min()

	std::vector<Vertex> vertices = terrain->generateVertices();

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

engine::AABB* engine::TerrainComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, engine::EditorProperty> engine::TerrainComponent::getPublicProperties()
{
	return m_terrain->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::TerrainComponent::getPropertySetters()
{
	return m_terrain->getPropertySetters();
}

void engine::TerrainComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
	auto it = m_propertySetters.find(key);
	if (it != m_propertySetters.end())
	{
		it->second(value);
		m_terrain->reSetup(); // Assuming all primitives have a reSetup() method
	}
}

void engine::TerrainComponent::setEnabled(bool enabled)
{
	ComponentBase<TerrainComponent>::setEnabled(enabled);

	m_terrain->setEnabled(enabled);
}