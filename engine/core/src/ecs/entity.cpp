#include "../../include/ecs/entity.h"

#include "../../include/ecs/transform_component.h"
#include "../../include/ecs/primitive_component.h"
#include "../../include/ecs/model_component.h"
#include "../../include/ecs/camera_component.h"
#include "../../include/ecs/light_component.h"

#include "../../include/managers/log_manager.h"

// constructor, expects just a name
engine::Entity::Entity(const std::string& _name)
	: name{ _name }, id { generateUniqueId() }
{
}

unsigned int engine::Entity::generateUniqueId()
{
	return rand() % 101;
}

engine::EntityType engine::Entity::getType()
{
	if (auto component = getComponent<ModelComponent>()) return engine::EntityType::model;
	if (auto component = getComponent<PrimitiveComponent>()) return engine::EntityType::primitive;
	if (auto component = getComponent<LightComponent>()) return engine::EntityType::light;
	if (auto component = getComponent<CameraComponent>()) return engine::EntityType::camera;

	return engine::EntityType::undefined;
}

std::string engine::Entity::getTypeName()
{
	auto entityType = getType();
	if (entityType == engine::EntityType::model) return "Model";
	if (entityType == engine::EntityType::primitive) return "Primitive";
	if (entityType == engine::EntityType::light) return "Light";
	if (entityType == engine::EntityType::camera) return "Camera";

	return "";
}

std::string engine::Entity::getTypeNameEx()
{
	if (auto modelComponent = getComponent<ModelComponent>())
	{
		return "Model";
	}
	else if (auto primitiveComponent = getComponent<PrimitiveComponent>())
	{
		auto primitive = primitiveComponent->getPrimitive();
		if (primitive)
		{
			if (primitive->getTypeID() == PrimitiveType::cube) {
				return "Cube primitive";
			}
			else if (primitive->getTypeID() == PrimitiveType::sphere) {
				return "Sphere primitive";
			}
			else if (primitive->getTypeID() == PrimitiveType::plane) {
				return "Plane primitive";
			}
			else if (primitive->getTypeID() == PrimitiveType::cylinder) {
				return "Cylinder primitive";
			}
			else if (primitive->getTypeID() == PrimitiveType::cone) {
				return "Cone primitive";
			}
			else if (primitive->getTypeID() == PrimitiveType::billboard) {
				return "Billboard primitive";
			}
		}

		return "Primitive";
	}
	else if (auto lightComponent = getComponent<LightComponent>())
	{
		auto light = lightComponent->getLight();
		if (light)
		{
			if (light->getTypeID() == LightType::directional) {
				return "Directional Light";
			}
			else if (light->getTypeID() == LightType::spot) {
				return "Spot Light";
			}
			else if (light->getTypeID() == LightType::point) {
				return "Point Light";
			}
			else if (light->getTypeID() == LightType::area) {
				return "Area Light";
			}
		}
		
		return "Light";
	}
	else if (auto cameraComponent = getComponent<CameraComponent>())
	{
		auto camera = cameraComponent->getCamera();
		if (camera)
		{
			if (camera->getTypeID() == CameraType::fly) {
				return "Fly camera";
			}
			else if (camera->getTypeID() == CameraType::fps) {
				return "FPS camera";
			}
			else if (camera->getTypeID() == CameraType::orbit) {
				return "Orbit camera";
			}
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
	glm::mat4 worldTrandform = parentTransform * getTransform().getLocalModelMatrix();
	setWorldTransform(worldTrandform);

	for (auto& child : children) {
		child->updateSelfAndChild(worldTrandform);
	}
}

//engine::SphereVolume engine::Entity::generateSphereBV(const Model& model)
//{
//	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
//	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
//
//	for (auto&& mesh : model.meshes)
//	{
//		for (auto&& vertex : mesh.vertices)
//		{
//			minAABB.x = std::min(minAABB.x, vertex.position.x);
//			minAABB.y = std::min(minAABB.y, vertex.position.y);
//			minAABB.z = std::min(minAABB.z, vertex.position.z);
//
//			maxAABB.x = std::max(maxAABB.x, vertex.position.x);
//			maxAABB.y = std::max(maxAABB.y, vertex.position.y);
//			maxAABB.z = std::max(maxAABB.z, vertex.position.z);
//		}
//	}
//
//	return engine::SphereVolume((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
//}


engine::Transform engine::Entity::getTransform()
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		return trsComponent->getTransform();
	}

	logger.warn("Entity getTransform() error, entity {} has no transform component", this->name);

	return Transform{};
}

void engine::Entity::setTransform(const engine::Transform& transform)
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		trsComponent->setTransform(transform);
	}
	else
	{
		logger.warn("Entity setTransform() error, entity {} has no transform component", this->name);
	}
}

glm::mat4& engine::Entity::getWorldTransform()
{
	static glm::mat4 empty{};
	
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		return trsComponent->getWorldTransformMatrix();
	}

	logger.warn("Entity getWorldTransform() error, entity {} has no transform component", this->name);

	return empty;
}

void engine::Entity::setWorldTransform(const glm::mat4& worldTransform)
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		trsComponent->setWorldTransformMatrix(worldTransform);
	}
	else
	{
		logger.warn("Entity setWorldTransform() error, entity {} has no transform component", this->name);
	}
}

engine::AABB* engine::Entity::getBoundingVolume()
{
	auto nnn = this->name;
	
	if (auto primitiveComponent = getComponent<engine::PrimitiveComponent>())
	{
		return primitiveComponent->getBoundingVolume();
	}
	else if (auto modelComponent = getComponent<engine::ModelComponent>())
	{
		return modelComponent->getBoundingVolume();
	}

	return nullptr;
}

engine::AABB engine::Entity::getGlobalAABB()
{
	auto transform = getTransform();
	auto worldTransform = getWorldTransform();

	auto boundingVolume = getBoundingVolume();

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

void engine::Entity::setEnabled(bool _enabled)
{
	if (this->getType() == EntityType::light)
	{
		auto component = getComponent<LightComponent>();
		if (component)
		{
			auto light = component->getLight();
			if (light)
			{
				// disable light
				light->setEnabled(_enabled);
			}
		}
	}
	
	// disable entity
	this->enabled = _enabled;
}