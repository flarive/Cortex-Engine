#pragma once

//#include <glad/glad.h>
//#include <glm/glm.hpp>

#include "common_defines.h"

#include "misc/noncopyable.h"

#include "model.h"
#include "camera.h"

#include "transform.h"
#include "bounding_volume_plane.h"

#include <list> //std::list
#include <array> //std::array
#include <memory> //std::unique_ptr

namespace engine
{
	struct Frustum
	{
		BoundingVolumePlane2 topFace;
		BoundingVolumePlane2 bottomFace;

		BoundingVolumePlane2 rightFace;
		BoundingVolumePlane2 leftFace;

		BoundingVolumePlane2 farFace;
		BoundingVolumePlane2 nearFace;
	};

	struct BoundingVolume
	{
		virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

		virtual bool isOnOrForwardPlane(const BoundingVolumePlane2& plane) const = 0;

		bool isOnFrustum(const Frustum& camFrustum) const
		{
			return (isOnOrForwardPlane(camFrustum.leftFace) &&
				isOnOrForwardPlane(camFrustum.rightFace) &&
				isOnOrForwardPlane(camFrustum.topFace) &&
				isOnOrForwardPlane(camFrustum.bottomFace) &&
				isOnOrForwardPlane(camFrustum.nearFace) &&
				isOnOrForwardPlane(camFrustum.farFace));
		};
	};

	struct SphereVolume : public BoundingVolume
	{
		glm::vec3 center{ 0.f, 0.f, 0.f };
		float radius{ 0.f };

		SphereVolume(const glm::vec3& inCenter, float inRadius)
			: BoundingVolume{}, center{ inCenter }, radius{ inRadius }
		{
		}

		bool isOnOrForwardPlane(const BoundingVolumePlane2& plane) const final
		{
			return plane.getSignedDistanceToPlane(center) > -radius;
		}

		bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
		{
			//Get global scale thanks to our transform
			const glm::vec3 globalScale = transform.getGlobalScale();

			//Get our global center with process it with the global model matrix of our transform
			const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

			//To wrap correctly our shape, we need the maximum scale scalar.
			const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

			//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
			SphereVolume globalSphere(globalCenter, radius * (maxScale * 0.5f));

			//Check Firstly the result that have the most chance to failure to avoid to call all functions.
			return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
		};
	};

	struct SquareAABB : public BoundingVolume
	{
		glm::vec3 center{ 0.f, 0.f, 0.f };
		float extent{ 0.f };

		SquareAABB(const glm::vec3& inCenter, float inExtent)
			: BoundingVolume{}, center{ inCenter }, extent{ inExtent }
		{
		}

		bool isOnOrForwardPlane(const BoundingVolumePlane2& plane) const final
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
			return -r <= plane.getSignedDistanceToPlane(center);
		}

		bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
		{
			//Get global scale thanks to our transform
			const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

			// Scaled orientation
			const glm::vec3 right = transform.getRight() * extent;
			const glm::vec3 up = transform.getUp() * extent;
			const glm::vec3 forward = transform.getForward() * extent;

			const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

			const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

			const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

			const SquareAABB globalAABB(globalCenter, std::max(std::max(newIi, newIj), newIk));

			return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.farFace));
		};
	};

	struct AABB : public BoundingVolume
	{
		glm::vec3 center{ 0.f, 0.f, 0.f };
		glm::vec3 extents{ 0.f, 0.f, 0.f };

		AABB(const glm::vec3& min, const glm::vec3& max)
			: BoundingVolume{}, center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
		{
		}

		AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
			: BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
		{
		}

		std::array<glm::vec3, 8> getVertice() const
		{
			std::array<glm::vec3, 8> vertice;
			vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
			vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
			vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
			vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
			vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
			vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
			vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
			vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
			return vertice;
		}

		//see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
		bool isOnOrForwardPlane(const BoundingVolumePlane2& plane) const final
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
				extents.z * std::abs(plane.normal.z);

			return -r <= plane.getSignedDistanceToPlane(center);
		}

		bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
		{
			//Get global scale thanks to our transform
			const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

			// Scaled orientation
			const glm::vec3 right = transform.getRight() * extents.x;
			const glm::vec3 up = transform.getUp() * extents.y;
			const glm::vec3 forward = transform.getForward() * extents.z;

			const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

			const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

			const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
				std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

			const AABB globalAABB(globalCenter, newIi, newIj, newIk);

			return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
				globalAABB.isOnOrForwardPlane(camFrustum.farFace));
		};
	};

	inline Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
	{
		Frustum     frustum;
		const float halfVSide = zFar * tanf(fovY * .5f);
		const float halfHSide = halfVSide * aspect;
		const glm::vec3 frontMultFar = zFar * cam.Front;

		frustum.nearFace = { cam.Position + zNear * cam.Front, cam.Front };
		frustum.farFace = { cam.Position + frontMultFar, -cam.Front };
		frustum.rightFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
		frustum.leftFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
		frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
		frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };
		return frustum;
	}

	inline AABB generateAABB(const std::shared_ptr<Model> model)
	{
		glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
		for (auto&& mesh : model->meshes)
		{
			for (auto&& vertex : mesh.vertices)
			{
				minAABB.x = std::min(minAABB.x, vertex.Position.x);
				minAABB.y = std::min(minAABB.y, vertex.Position.y);
				minAABB.z = std::min(minAABB.z, vertex.Position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
			}
		}
		return AABB(minAABB, maxAABB);
	}

	inline SphereVolume generateSphereBV(const Model& model)
	{
		glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
		for (auto&& mesh : model.meshes)
		{
			for (auto&& vertex : mesh.vertices)
			{
				minAABB.x = std::min(minAABB.x, vertex.Position.x);
				minAABB.y = std::min(minAABB.y, vertex.Position.y);
				minAABB.z = std::min(minAABB.z, vertex.Position.z);

				maxAABB.x = std::max(maxAABB.x, vertex.Position.x);
				maxAABB.y = std::max(maxAABB.y, vertex.Position.y);
				maxAABB.z = std::max(maxAABB.z, vertex.Position.z);
			}
		}

		return SphereVolume((maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB));
	}

	class Entity
	{
	public:
		//Scene graph
		std::list<std::shared_ptr<Entity>> children;
		
		
		Entity* parent = nullptr;

		//Space information
		Transform transform;

		std::string name;
		std::shared_ptr<Model> model;
		std::unique_ptr<AABB> boundingVolume;

		// constructor, expects a filepath to a 3D model.
		Entity(const std::string& _name, std::shared_ptr<Model> _model) : name{ _name }, model{ _model }
		{
			boundingVolume = std::make_unique<AABB>(generateAABB(_model));
			//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
		}

		// constructor, expects a filepath to a 3D model.
		Entity(std::shared_ptr<Model> _model) : model{ _model }
		{
			boundingVolume = std::make_unique<AABB>(generateAABB(_model));
			//boundingVolume = std::make_unique<Sphere>(generateSphereBV(model));
		}

		AABB getGlobalAABB()
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

			return AABB(globalCenter, newIi, newIj, newIk);
		}

		//Add child.
		//Argument input is argument of any constructor that you create. By default you can use the default constructor and don't put argument input.
		template<typename... TArgs>
		void addChild(const std::string& name, TArgs&... args)
		{
			children.emplace_back(std::make_unique<Entity>(name, args...));
			children.back()->parent = this;
		}

		//Update transform if it was changed
		void updateSelfAndChild()
		{
			if (transform.isDirty()) {
				forceUpdateSelfAndChild();
				return;
			}

			for (auto&& child : children)
			{
				child->updateSelfAndChild();
			}
		}

		//Force update of transform even if local space don't change
		void forceUpdateSelfAndChild()
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


		void drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total)
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
	};
}