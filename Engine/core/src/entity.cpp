#include "../include/entity.h"

#include "../include/primitives/cube.h"

// constructor, expects just a name
engine::Entity::Entity(const std::string& _name) : name{ _name }
{
}

// constructor, expects a filepath to a 3D model and a transform
engine::Entity::Entity(const std::string& _name, std::shared_ptr<Model> _model, Transform _transform) : name{ _name }, model{ _model }, transform{ _transform }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}

// constructor, expects a filepath to a 3D model.
engine::Entity::Entity(const std::string& _name, std::shared_ptr<Model> _model) : name{ _name }, model{ _model }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}

// constructor, expects a filepath to a 3D model.
engine::Entity::Entity(std::shared_ptr<Model> _model) : model{ _model }
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_model));
	//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
}


// constructor, expects a filepath to a 3D model.
engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Primitive> _primitive, Transform _transform)
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}

engine::Entity::Entity(const std::string& _name, std::shared_ptr<engine::Primitive> _primitive)
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}

engine::Entity::Entity(std::shared_ptr<engine::Primitive> _primitive)
{
	boundingVolume = std::make_unique<AABB>(generateAABB(_primitive));
}





engine::AABB engine::Entity::getGlobalAABB()
{
	//Get global scale thanks to our transform
	const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(boundingVolume->center, 1.f) };

	// Scaled orientation
	const glm::vec3 right = transform.getRight() * boundingVolume->extents.x;
	const glm::vec3 up = transform.getUp() * boundingVolume->extents.y;
	const glm::vec3 forward = transform.getForward() * boundingVolume->extents.z;

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

// Add a child using an existing Entity instance
void engine::Entity::addChild(std::shared_ptr< engine::Entity> entity)
{
	if (entity->parent != nullptr) {
		// Optional: throw, log warning, or remove from previous parent
		return;
	}

	entity->parent = this;
	children.emplace_back(std::move(entity));
}

//Update transform if it was changed
void engine::Entity::updateSelfAndChild()
{
	if (transform.isDirty()) {
		// Update current node and propagate to children
		forceUpdateSelfAndChild();
	}
	else {
		// Still update children because they may be dirty themselves
		for (auto&& child : children)
		{
			child->updateSelfAndChild();
		}
	}
}

//Force update of transform even if local space don't change
void engine::Entity::forceUpdateSelfAndChild()
{
	if (parent)
		transform.computeModelMatrix(parent->transform.getModelMatrix());
	else
		transform.computeModelMatrix();

	for (auto&& child : children)
	{
		child->forceUpdateSelfAndChild();
	}
}

void engine::Entity::drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
{
	if (boundingVolume->isOnFrustum(frustum, transform))
	{
		ourShader.setMat4("model", transform.getModelMatrix());
		model->draw(ourShader);
		display++;
	}
	total++;

	for (auto&& child : children)
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