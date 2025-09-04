#include "../include/entity.h"

#include "../include/lights/point_light.h"
#include "../include/lights/spot_light.h"
#include "../include/lights/directional_light.h"

#include "../include/primitives/cube.h"
#include "../include/primitives/sphere.h"
#include "../include/primitives/plane.h"
#include "../include/primitives/cone.h"
#include "../include/primitives/cylinder.h"
#include "../include/primitives/billboard.h"

#include "../include/cameras/fly_camera.h"
#include "../include/cameras/fps_camera.h"
#include "../include/cameras/orbit_camera.h"



// constructor, expects just a name
engine::Entity::Entity(const std::string& _name) : name{ _name }, id { generateUniqueId() }
{
}

// constructor, expects a filepath to a 3D model and a transform
engine::Entity::Entity(const std::string& _name, std::shared_ptr<Model> _model, Transform _transform) : name{ _name }, id{ generateUniqueId() }, model{ _model }, transform{ _transform }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}

// constructor, expects a filepath to a 3D model.
engine::Entity::Entity(const std::string& _name, std::shared_ptr<Model> _model) : name{ _name }, id{ generateUniqueId() }, model{ _model }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}

// constructor, expects a filepath to a 3D model.
engine::Entity::Entity(std::shared_ptr<Model> _model) : id{ generateUniqueId() }, model{ _model }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}



engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Primitive> _primitive, Transform _transform) : name{ _name }, id{ generateUniqueId() }, primitive{ _primitive }, transform{ _transform }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}

engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Primitive> _primitive) : name{ _name }, id{ generateUniqueId() }, primitive{ _primitive }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}

engine::Entity::Entity(std::shared_ptr<engine::Primitive> _primitive) : id{ generateUniqueId() }, primitive{ _primitive }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}



engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Light> _light, Transform _transform) : name{ _name }, id{ generateUniqueId() }, light{ _light }, transform{ _transform }
{
	// set light position from transform position
	_light->position = _transform.getLocalPosition();
	
	boundingVolume = std::make_unique<AABB>(generateAABB(_light));
}

engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Light> _light) : name{ _name }, id{ generateUniqueId() }, light{ _light }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_light));
}

engine::Entity::Entity(std::shared_ptr<engine::Light> _light) : id{ generateUniqueId() }, light{ _light }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_light));
}


engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Camera> _camera, Transform _transform) : name{ _name }, id{ generateUniqueId() }, camera{ _camera }, transform{ _transform }
{
	// set camera position from transform position
	//_camera->position = _transform.getLocalPosition();

	boundingVolume = std::make_unique<AABB>(generateAABB(_camera));
}

engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Camera> _camera) : name{ _name }, id{ generateUniqueId() }, camera{ _camera }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_camera));
}

engine::Entity::Entity(std::shared_ptr<engine::Camera> _camera) : id{ generateUniqueId() }, camera{ _camera }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_camera));
}

engine::AABB engine::Entity::getGlobalAABB()
{
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
		//if (std::dynamic_pointer_cast<engine::LegacyCamera>(this->camera))
		//{
		//	return "Legacy camera";
		//}

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
	if (parent == nullptr) {
		worldTransform = parentTransform * transform.getLocalModelMatrix();
	}
	else {
		// Still need to recompute in case parent changed
		worldTransform = parentTransform * transform.getLocalModelMatrix();
	}

	for (auto& child : children) {
		child->updateSelfAndChild(worldTransform);
	}
}

void engine::Entity::forceUpdateSelfAndChild()
{
	if (parent)
		updateSelfAndChild(parent->worldTransform);
	else
		updateSelfAndChild(glm::mat4(1.0f)); // root starts with identity
}

void engine::Entity::drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
{
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

engine::AABB engine::Entity::generateAABB(const std::shared_ptr<Model> model)
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


engine::AABB engine::Entity::generateAABB(const std::shared_ptr<Primitive> primitive)
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

engine::AABB engine::Entity::generateAABB(const std::shared_ptr<Light> light)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
}

engine::AABB engine::Entity::generateAABB(const std::shared_ptr<Camera> camera)
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

	return engine::AABB(minAABB, maxAABB);
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