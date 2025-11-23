#include "../../include/ecs/primitive_component.h"

#include "../../include/singleton.h"

#include "../../include/primitives/cube.h"
#include "../../include/primitives/sphere.h"
#include "../../include/primitives/plane.h"
#include "../../include/primitives/cylinder.h"
#include "../../include/primitives/cone.h"
#include "../../include/primitives/billboard.h"

engine::PrimitiveComponent::PrimitiveComponent(std::shared_ptr<Primitive> primitive) : m_primitive(primitive)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(primitive));

	// load light cube debug shader
	m_lightDebugShader.init("primitive_boundingbox_debug", "shaders/debug/debug_light.vert", "shaders/debug/debug_light.frag");

	auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 0.5f));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<Cube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup(matDebugLight);


	// Initialize property setters based on primitive type
	m_propertySetters = m_primitive->getPropertySetters();
}

void engine::PrimitiveComponent::init(Transform& transform)
{
	m_primitive->position = transform.getLocalPosition();
	m_primitive->rotation = transform.getLocalRotation();
	m_primitive->scale = transform.getLocalScale();
}

void engine::PrimitiveComponent::update(Transform& transform)
{

}

void engine::PrimitiveComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
	m_primitive->draw(shader, projection, view, worldTransformMatrix, localTransform);

	auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
	SceneSettings& sceneSettings = singleton->sceneSettings();

	if (sceneSettings.drawBoundingBoxesVisualHelpers)
	{
		// Pass model matrix to shader
		m_lightDebugShader.use();
		m_lightDebugShader.setMat4("projection", projection);
		m_lightDebugShader.setMat4("view", view);
		m_lightDebugShader.setVec4("customColor", m_debug_boundingBox->getMaterial()->getAmbientColor());
		m_debug_boundingBox->draw(m_lightDebugShader, projection, view, worldTransformMatrix, localTransform);
	}
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

engine::AABB* engine::PrimitiveComponent::getBoundingVolume()
{
	return m_boundingVolume.get();
}

engine::ordered_map<std::string, std::variant<int, std::string, float, bool>> engine::PrimitiveComponent::getPublicProperties()
{
	return m_primitive->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(float)>> engine::PrimitiveComponent::getPropertySetters()
{
	return m_primitive->getPropertySetters();
}

void engine::PrimitiveComponent::setProperty(const std::string& key, float value)
{
	auto it = m_propertySetters.find(key);
	if (it != m_propertySetters.end())
	{
		it->second(value);
		m_primitive->reSetup(); // Assuming all primitives have a reSetup() method
	}
}