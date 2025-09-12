#include "../../include/ecs/entity.h"

#include "../../include/lights/point_light.h"
#include "../../include/lights/spot_light.h"
#include "../../include/lights/directional_light.h"

#include "../../include/primitives/cube.h"
#include "../../include/primitives/sphere.h"
#include "../../include/primitives/plane.h"
#include "../../include/primitives/cone.h"
#include "../../include/primitives/cylinder.h"
#include "../../include/primitives/billboard.h"

#include "../../include/cameras/fly_camera.h"
#include "../../include/cameras/fps_camera.h"
#include "../../include/cameras/orbit_camera.h"


#include "../../include/ecs/transform_component.h"

#include "../../include/aabb.h"


// constructor, expects just a name
engine::Entity::Entity(const std::string& _name)
	: name{ _name }, id { generateUniqueId() }
{
}

// constructor, expects just a name and a transform (TEMP)
engine::Entity::Entity(const std::string& _name, Transform _transform)
	: name{ _name }, id{ generateUniqueId() }//, transform{ _transform }
{
}

engine::AABB engine::Entity::getGlobalAABB()
{
	auto transform = getTransform();
	auto worldTransform = getWorldTransform();
	
	// Transform local center into world space
	const glm::vec3 globalCenter{ worldTransform * glm::vec4(boundingVolume->center, 1.f) };

	// Get scaled orientation axes from world transform
	const glm::vec3 right = transform.getRight(worldTransform) * boundingVolume->extents.x;
	const glm::vec3 up = transform.getUp(worldTransform) * boundingVolume->extents.y;
	const glm::vec3 forward = transform.getForward(worldTransform) * boundingVolume->extents.z;

	// Project onto global axes
	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	return engine::AABB(globalCenter, newIi, newIj, newIk);
}

unsigned int engine::Entity::generateUniqueId()
{
	return rand() % 101;
}

engine::EntityType engine::Entity::getType()
{
	if (this->model)
		return engine::EntityType::model;
	else if (this->primitive)
		return engine::EntityType::primitive;
	else if (this->light)
		return engine::EntityType::light;
	else if (this->camera)
		return engine::EntityType::camera;

	return engine::EntityType::undefined;
}

std::string engine::Entity::getTypeName()
{
	if (this->model)
		return "Model";
	else if (this->primitive)
		return "Primitive";
	else if (this->light)
		return "Light";
	else if (this->camera)
		return "Camera";

	return "";
}

std::string engine::Entity::getTypeNameEx()
{
	if (this->model)
	{
		return "Model";
	}
	else if (this->primitive)
	{
		if (std::dynamic_pointer_cast<engine::Cube>(this->primitive))
		{
			return "Cube primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Sphere>(this->primitive))
		{
			return "Sphere primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Plane>(this->primitive))
		{
			return "Plane primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Cylinder>(this->primitive))
		{
			return "Cylinder primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Cone>(this->primitive))
		{
			return "Cone primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Billboard>(this->primitive))
		{
			return "Billboard primitive";
		}
		
		return "Primitive";
	}
	else if (this->light)
	{
		if (std::dynamic_pointer_cast<engine::DirectionalLight>(this->light))
		{
			return "Directional Light";
		}
		else if (std::dynamic_pointer_cast<engine::SpotLight>(this->light))
		{
			return "Spot Light";
		}
		else if (std::dynamic_pointer_cast<engine::PointLight>(this->light))
		{
			return "Point Light";
		}
		
		return "Light";
	}
	else if (this->camera)
	{
		if (std::dynamic_pointer_cast<engine::FlyCamera>(this->camera))
		{
			return "Fly camera";
		}
		else if (std::dynamic_pointer_cast<engine::FpsCamera>(this->camera))
		{
			return "FPS camera";
		}
		else if (std::dynamic_pointer_cast<engine::OrbitCamera>(this->camera))
		{
			return "Orbit camera";
		}

		return "Camera";
	}

	return "";
}

// Add a child using an existing Entity instance
void engine::Entity::addChild(std::shared_ptr<engine::Entity> entity)
{
	if (entity->parent != nullptr) {
		// Optional: throw, log warning, or remove from previous parent
		return;
	}

	entity->parent = this;
	children.emplace_back(std::move(entity));
}

// Recursively update world transforms
void engine::Entity::updateSelfAndChild(const glm::mat4& parentTransform)
{
	//Transform transform = getTransform();
	//glm::mat4 worldTransform = getWorldTransform();
	
	
	if (parent == nullptr) {
		setWorldTransform(parentTransform * getTransform().getLocalModelMatrix());
	}
	else {
		// Still need to recompute in case parent changed
		setWorldTransform(parentTransform * getTransform().getLocalModelMatrix());
	}

	for (auto& child : children) {
		child->updateSelfAndChild(getWorldTransform());
	}
}

void engine::Entity::forceUpdateSelfAndChild()
{
	if (parent)
		updateSelfAndChild(parent->getWorldTransform());
	else
		updateSelfAndChild(glm::mat4(1.0f)); // root starts with identity
}

void engine::Entity::drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
{
	auto worldTransform = getWorldTransform();

	if (boundingVolume->isOnFrustum(frustum, worldTransform))
	{
		ourShader.setMat4("model", worldTransform);
		if (model) model->draw(ourShader);
		display++;
	}
	total++;

	for (auto& child : children)
	{
		child->drawSelfAndChild(frustum, ourShader, display, total);
	}
}

engine::SphereVolume engine::Entity::generateSphereBV(const Model& model)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	for (auto&& mesh : model.meshes)
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

	return engine::SphereVolume((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
}


engine::Transform engine::Entity::getTransform()
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		return trsComponent->getTransform();
	}
}

void engine::Entity::setTransform(const engine::Transform& transform)
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		trsComponent->setTransform(transform);
	}
}

glm::mat4 engine::Entity::getWorldTransform()
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		return trsComponent->getWorldTransformMatrix();
	}
}

void engine::Entity::setWorldTransform(const glm::mat4& worldTransform)
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		trsComponent->setWorldTransformMatrix(worldTransform);
	}
}

//std::unique_ptr<engine::AABB> engine::Entity::getBoundingVolume()
//{
//	if (auto trsComponent = getComponent<engine::PrimitiveComponent>())
//	{
//		return trsComponent->getWorldTransformMatrix();
//	}
//}
