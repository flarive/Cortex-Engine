#include "../../include/ecs/primitive_component.h"

#include "../../include/singleton.h"

#include "../../include/primitives/cube.h"
#include "../../include/primitives/sphere.h"
#include "../../include/primitives/plane.h"
#include "../../include/primitives/cylinder.h"
#include "../../include/primitives/cone.h"
#include "../../include/primitives/billboard.h"

engine::PrimitiveComponent::PrimitiveComponent(std::shared_ptr<Primitive> primitive)
    : m_primitive(primitive)
{
	m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(primitive));

	// load light cube debug shader
	m_lightDebugShader.init("primitive_boundingbox_debug", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");

	auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 0.5f));

	auto [width, height, depth] = m_boundingVolume->getAABBDimensions();
	m_debug_boundingBox = std::make_unique<Cube>(width, height, depth); // Cube at origin with dimensions of the AABB
	m_debug_boundingBox->setup(matDebugLight);


	// Initialize property setters based on primitive type

	// usefull to have static_pointer_cast to work
	auto primitiveType = m_primitive->getTypeID();

	if (primitiveType == PrimitiveType::sphere)
	{
		if (auto sphere = std::static_pointer_cast<Sphere>(m_primitive))
		{
			m_propertySetters = {
				{"radius", [sphere](float value) { sphere->radius = value; }},
				{"uvscale", [sphere](float value) { sphere->getUvScale() = value; }}
			};
		}
	}
	else if (primitiveType == PrimitiveType::cube)
	{
		if (auto cube = std::static_pointer_cast<Cube>(m_primitive))
		{
			m_propertySetters = {
				{"uvscale", [cube](float value) { cube->getUvScale() = value; }}
			};
		}
	}
	else if (primitiveType == PrimitiveType::plane)
	{
		if (auto plane = std::static_pointer_cast<Plane>(m_primitive))
		{
			m_propertySetters = {
				{"uvscale", [plane](float value) { plane->getUvScale() = value; }}
			};
		}
	}
	else if (primitiveType == PrimitiveType::cylinder)
	{
		if (auto cylinder = std::static_pointer_cast<Cylinder>(m_primitive))
		{
			m_propertySetters = {
				{"radius", [cylinder](float value) { cylinder->radius = value; }},
				{"height", [cylinder](float value) { cylinder->height = value; }},
				{"uvscale", [cylinder](float value) { cylinder->getUvScale() = value; }}
			};
		}
	}
	else if (primitiveType == PrimitiveType::cone)
	{
		if (auto cone = std::static_pointer_cast<Cone>(m_primitive))
		{
			m_propertySetters = {
				{"radius", [cone](float value) { cone->radius = value; }},
				{"height", [cone](float value) { cone->height = value; }},
				{"uvscale", [cone](float value) { cone->getUvScale() = value; }}
			};
		}
	}
	else if (primitiveType == PrimitiveType::billboard)
	{
		if (auto billboard = std::static_pointer_cast<Billboard>(m_primitive))
		{
			m_propertySetters = {
				{"uvscale", [billboard](float value) { billboard->getUvScale() = value; }}
			};
		}
	}
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
	m_primitive->draw(shader, worldTransformMatrix, localTransform);

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
		m_debug_boundingBox->draw(m_lightDebugShader, worldTransformMatrix, localTransform);
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

std::vector<engine::KeyValuePair> engine::PrimitiveComponent::getPublicProperties()
{
	// usefull to have static_pointer_cast to work
	auto primitiveType = m_primitive->getTypeID();

	if (primitiveType == PrimitiveType::sphere)
	{
		if (auto sphere = std::static_pointer_cast<Sphere>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", sphere->radius },
				engine::KeyValuePair{ "uvscale", sphere->getUvScale() }
			};
		}
	}
	else if (primitiveType == PrimitiveType::cube)
	{
		if (auto cube = std::static_pointer_cast<Cube>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "uvscale", cube->getUvScale() }
			};
		}
	}
	else if (primitiveType == PrimitiveType::plane)
	{
		if (auto plane = std::static_pointer_cast<Plane>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "uvscale", plane->getUvScale() }
			};
		}
	}
	else if (primitiveType == PrimitiveType::cylinder)
	{
		if (auto cylinder = std::static_pointer_cast<Cylinder>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", cylinder->radius },
				engine::KeyValuePair{ "height", cylinder->height },
				engine::KeyValuePair{ "uvscale", cylinder->getUvScale() }
			};
		}
	}
	else if (primitiveType == PrimitiveType::cone)
	{
		if (auto cone = std::static_pointer_cast<Cone>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", cone->radius },
				engine::KeyValuePair{ "height", cone->height },
				engine::KeyValuePair{ "uvscale", cone->getUvScale() }
			};
		}
	}
	else if (primitiveType == PrimitiveType::billboard)
	{
		if (auto billboard = std::static_pointer_cast<Billboard>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "uvscale", billboard->getUvScale() }
			};
		}
	}
	
	return{};
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