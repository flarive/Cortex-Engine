#pragma once

#include "../common_defines.h"

//#include "../misc/noncopyable.h"

#include "../model.h"
#include "../primitives/primitive.h"
#include "../lights/light.h"
#include "../cameras/camera.h"
#include "../transform.h"
#include "../frustrum.h"
#include "../bounding_volume.h"
#include "../aabb.h"
#include "../ecs/component.h"

#include <list>
#include <memory>

namespace engine
{
	enum class EntityType { undefined = 0, model = 1, primitive = 2, light = 3, camera = 4 };

	class Entity
	{
	public:
		unsigned int id{};
		std::string name{};
		bool visible{ true };

		// To remove
		//std::shared_ptr<Model> model{};
		//std::shared_ptr<Primitive> primitive{};
		//std::shared_ptr<Light> light{};
		//std::shared_ptr<Camera> camera{};
		std::unique_ptr<AABB> boundingVolume{};

		// Scene graph
		Entity* parent{};
		std::list<std::shared_ptr<Entity>> children{};


		// Components
		std::unordered_map<unsigned int, std::shared_ptr<Component>> components{};

		// To remove
		//Transform transform{};       // local position/rotation/scale
		//glm::mat4 worldTransform{};  // should be full parent * local


		// constructor, expects just a name
		Entity(const std::string& _name);
		Entity(const std::string& _name, Transform _transform);



		EntityType getType();
		std::string getTypeName();
		std::string getTypeNameEx();


		Transform getTransform();
		void setTransform(const engine::Transform& transform);
		
		glm::mat4 getWorldTransform();
		void setWorldTransform(const glm::mat4& worldTransform);

		
		

#pragma region Entities

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
		void updateSelfAndChild(const glm::mat4& parentTransform = glm::mat4(1.0f));

		//Force update of transform even if local space don't change
		void forceUpdateSelfAndChild();

#pragma endregion



#pragma region Components

		template<typename T, typename... Args>
			requires std::derived_from<T, ComponentBase<T>>
		T* addComponent(Args&&... args) {
			auto component = std::make_shared<T>(std::forward<Args>(args)...);
			unsigned int typeID = T::getStaticTypeID(); // Static call
			components[typeID] = component;
			return static_cast<T*>(component.get());
		}

		template<typename T>
			requires std::derived_from<T, ComponentBase<T>>
		std::shared_ptr<T> getComponent() {
			unsigned int typeID = T::getStaticTypeID(); // Static call
			auto it = components.find(typeID);
			if (it != components.end()) {
				return std::static_pointer_cast<T>(it->second);
			}
			return nullptr;
		}



#pragma endregion


	private:

		unsigned int generateUniqueId();
		AABB getGlobalAABB();

		void drawSelfAndChild(const Frustum& frustum, Shader& ourShader, unsigned int& display, unsigned int& total);

		SphereVolume generateSphereBV(const Model& model);

		//std::unique_ptr<AABB> getBoundingVolume();
	};
}