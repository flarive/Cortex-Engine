#include "../../include/ecs/primitive_component.h"

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

	// Initialize property setters based on primitive type
	if (auto sphere = std::static_pointer_cast<Sphere>(m_primitive))
	{
		m_propertySetters = {
			{"radius", [sphere](float value) { sphere->radius = value; }},
		};
	}
	else if (auto cylinder = std::static_pointer_cast<Cylinder>(m_primitive))
	{
		m_propertySetters = {
			{"radius", [cylinder](float value) { cylinder->radius = value; }},
			{"height", [cylinder](float value) { cylinder->height = value; }}
		};
	}
	else if (auto cone = std::static_pointer_cast<Cone>(m_primitive))
	{
		m_propertySetters = {
			{"radius", [cone](float value) { cone->radius = value; }},
			{"height", [cone](float value) { cone->height = value; }}
		};

		std::function<void(float)> func = [cone](float value) { cone->radius = value; };
	}


	
	

	//m_propertySetters.emplace("pppp", &m_primitive{
	//m_primitive->uvScale = value;
	//	});



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

void engine::PrimitiveComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform)
{
	m_primitive->draw(shader, worldTransformMatrix, localTransform);
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
	auto primitiveType = m_primitive->getTypeID();

	if (primitiveType == PrimitiveType::sphere)
	{
		if (auto sphere = std::static_pointer_cast<Sphere>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", sphere->radius }
			};
		}
	}
	else if (primitiveType == PrimitiveType::cylinder)
	{
		if (auto cylinder = std::static_pointer_cast<Cylinder>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", cylinder->radius },
				engine::KeyValuePair{ "height", cylinder->height }
			};
		}
	}
	else if (primitiveType == PrimitiveType::cone)
	{
		if (auto cone = std::static_pointer_cast<Cone>(m_primitive))
		{
			return {
				engine::KeyValuePair{ "radius", cone->radius },
				engine::KeyValuePair{ "height", cone->height }
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