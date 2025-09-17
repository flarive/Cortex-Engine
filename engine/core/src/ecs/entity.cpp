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
#include "../../include/ecs/primitive_component.h"
#include "../../include/ecs/model_component.h"
#include "../../include/ecs/camera_component.h"
#include "../../include/ecs/light_component.h"

#include "../../include/aabb.h"
#include "../../include/misc/log_manager.h"


// constructor, expects just a name
engine::Entity::Entity(const std::string& _name)
	: name{ _name }, id { generateUniqueId() }
{
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

unsigned int engine::Entity::generateUniqueId()
{
	return rand() % 101;
}

engine::EntityType engine::Entity::getType()
{
	if (auto modelComponent = getComponent<ModelComponent>())
	{
		return engine::EntityType::model;
	}
	else if (auto primitiveComponent = getComponent<PrimitiveComponent>())
	{
		return engine::EntityType::primitive;
	}
	else if (auto lightComponent = getComponent<LightComponent>())
	{
		return engine::EntityType::light;
	}
	else if (auto cameraComponent = getComponent<CameraComponent>())
	{
		return engine::EntityType::camera;
	}

	return engine::EntityType::undefined;
}

std::string engine::Entity::getTypeName()
{
	auto entityType = getType();
	
	if (entityType == engine::EntityType::model)
	{
		return "Model";
	}
	else if (entityType == engine::EntityType::primitive)
	{
		return "Primitive";
	}
	else if (entityType == engine::EntityType::light)
	{
		return "Light";
	}
	else if (entityType == engine::EntityType::camera)
	{
		return "Camera";
	}
	
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
		if (std::dynamic_pointer_cast<engine::Cube>(primitiveComponent->getPrimitive()))
		{
			return "Cube primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Sphere>(primitiveComponent->getPrimitive()))
		{
			return "Sphere primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Plane>(primitiveComponent->getPrimitive()))
		{
			return "Plane primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Cylinder>(primitiveComponent->getPrimitive()))
		{
			return "Cylinder primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Cone>(primitiveComponent->getPrimitive()))
		{
			return "Cone primitive";
		}
		else if (std::dynamic_pointer_cast<engine::Billboard>(primitiveComponent->getPrimitive()))
		{
			return "Billboard primitive";
		}
		
		return "Primitive";
	}
	else if (auto lightComponent = getComponent<LightComponent>())
	{
		if (std::dynamic_pointer_cast<engine::DirectionalLight>(lightComponent->getLight()))
		{
			return "Directional Light";
		}
		else if (std::dynamic_pointer_cast<engine::SpotLight>(lightComponent->getLight()))
		{
			return "Spot Light";
		}
		else if (std::dynamic_pointer_cast<engine::PointLight>(lightComponent->getLight()))
		{
			return "Point Light";
		}
		
		return "Light";
	}
	else if (auto cameraComponent = getComponent<CameraComponent>())
	{
		if (std::dynamic_pointer_cast<engine::FlyCamera>(cameraComponent->getCamera()))
		{
			return "Fly camera";
		}
		else if (std::dynamic_pointer_cast<engine::FpsCamera>(cameraComponent->getCamera()))
		{
			return "FPS camera";
		}
		else if (std::dynamic_pointer_cast<engine::OrbitCamera>(cameraComponent->getCamera()))
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
	glm::mat4 worldTrandform = parentTransform * getTransform().getLocalModelMatrix();
	setWorldTransform(worldTrandform);

	for (auto& child : children) {
		child->updateSelfAndChild(worldTrandform);
	}
}

void engine::Entity::forceUpdateSelfAndChild()
{
	if (parent)
		updateSelfAndChild(parent->getWorldTransform());
	else
		updateSelfAndChild(glm::mat4(1.0f)); // root starts with identity
}

//engine::Frustum engine::Entity::createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
//{
//	Frustum     frustum;
//	const float halfVSide = zFar * tanf(fovY * .5f);
//	const float halfHSide = halfVSide * aspect;
//	const glm::vec3 frontMultFar = zFar * cam.front;
//
//	frustum.nearFace = { cam.position + zNear * cam.front, cam.front };
//	frustum.farFace = { cam.position + frontMultFar, -cam.front };
//	frustum.rightFace = { cam.position, glm::cross(frontMultFar - cam.right * halfHSide, cam.up) };
//	frustum.leftFace = { cam.position, glm::cross(cam.up, frontMultFar + cam.right * halfHSide) };
//	frustum.topFace = { cam.position, glm::cross(cam.right, frontMultFar - cam.up * halfVSide) };
//	frustum.bottomFace = { cam.position, glm::cross(frontMultFar + cam.up * halfVSide, cam.right) };
//	return frustum;
//}

/// <summary>
/// ???????????????????????????????????????
/// </summary>
/// <param name="frustum"></param>
/// <param name="ourShader"></param>
/// <param name="display"></param>
/// <param name="total"></param>
void engine::Entity::drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
{
	auto worldTransform = getWorldTransform();

	if (getBoundingVolume()->isOnFrustum(frustum, worldTransform))
	{
		ourShader.setMat4("model", worldTransform);

		if (auto modelComponent = getComponent<ModelComponent>())
		{
			if (auto model = modelComponent->getModel())
				model->draw(ourShader);
		}

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

glm::mat4 engine::Entity::getWorldTransform()
{
	auto trsComponent = getComponent<engine::TransformComponent>();
	if (trsComponent)
	{
		return trsComponent->getWorldTransformMatrix();
	}

	logger.warn("Entity getWorldTransform() error, entity {} has no transform component", this->name);

	return glm::mat4{};
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

std::unique_ptr<engine::AABB> engine::Entity::getBoundingVolume()
{
	if (auto primitiveComponent = getComponent<engine::PrimitiveComponent>())
	{
		return primitiveComponent->getBoundingVolume();
	}
	else if (auto modelComponent = getComponent<engine::ModelComponent>())
	{
		return modelComponent->getBoundingVolume();
	}
	else if (auto cameraComponent = getComponent<engine::CameraComponent>())
	{
		return cameraComponent->getBoundingVolume();
	}
	else if (auto lightComponent = getComponent<engine::LightComponent>())
	{
		return lightComponent->getBoundingVolume();
	}

	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return std::make_unique<engine::AABB>(minAABB, maxAABB);
}
