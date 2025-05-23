#pragma once

#include "common_defines.h"

#include "misc/noncopyable.h"

#include "model.h"
#include "primitives/primitive.h"
#include "transform.h"
#include "frustrum.h"
#include "bounding_volume.h"
#include "aabb.h"

#include <list>
#include <memory>

namespace engine
{
	//inline Frustum createFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
	//{
	//	Frustum     frustum;
	//	const float halfVSide = zFar * tanf(fovY * .5f);
	//	const float halfHSide = halfVSide * aspect;
	//	const glm::vec3 frontMultFar = zFar * cam.Front;

	//	frustum.nearFace = { cam.Position + zNear * cam.Front, cam.Front };
	//	frustum.farFace = { cam.Position + frontMultFar, -cam.Front };
	//	frustum.rightFace = { cam.Position, glm::cross(frontMultFar - cam.Right * halfHSide, cam.Up) };
	//	frustum.leftFace = { cam.Position, glm::cross(cam.Up, frontMultFar + cam.Right * halfHSide) };
	//	frustum.topFace = { cam.Position, glm::cross(cam.Right, frontMultFar - cam.Up * halfVSide) };
	//	frustum.bottomFace = { cam.Position, glm::cross(frontMultFar + cam.Up * halfVSide, cam.Right) };
	//	return frustum;
	//}

	class Entity
	{
	public:
		//Scene graph
		std::list<std::shared_ptr<Entity>> children{};
		
		
		Entity* parent{};

		//Space information
		Transform transform{};

		std::string name{};
		std::shared_ptr<Model> model{};
		std::shared_ptr<Primitive> primitive{};
		std::unique_ptr<AABB> boundingVolume{};


		// constructor, expects just a name
		Entity(const std::string& _name);


		Entity(const std::string& _name, std::shared_ptr<Model> _model, Transform _transform);
		Entity(const std::string& _name, std::shared_ptr<Model> _model);
		Entity(std::shared_ptr<Model> _model);

		

		Entity(const std::string& _name, std::shared_ptr<Primitive> _primitive, Transform _transform);
		Entity(const std::string& _name, std::shared_ptr<Primitive> _primitive);
		Entity(std::shared_ptr<Primitive> _primitive);


		// Add child using Entity constructor
		// Argument input is argument of any constructor that you create. By default you can use the default constructor and don't put argument input.
		template<typename... TArgs>
		void addChild(const std::string& name, TArgs&&... args)
		{
			children.emplace_back(std::make_shared<Entity>(name, std::forward<TArgs>(args)...));
			children.back()->parent = this;
		}

		// Add a child using an existing Entity instance
		void addChild(std::shared_ptr<Entity> entity);

		//Update transform if it was changed
		void updateSelfAndChild();

		//Force update of transform even if local space don't change
		void forceUpdateSelfAndChild();


	private:
		AABB getGlobalAABB();

		void drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total);

		AABB generateAABB(const std::shared_ptr<Model> model);
		AABB generateAABB(const std::shared_ptr<Primitive> primitive);

		SphereVolume generateSphereBV(const Model& model);
	};
}