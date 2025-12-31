#pragma once

#include "../common_defines.h"

#include "../models/model.h"
#include "../transform.h"
#include "../aabb.h"
#include "../ecs/component.h"

#include <list>
#include <memory>

namespace engine
{
	enum class EntityType { undefined = 0, model = 1, primitive = 2, light = 3, camera = 4 };

	class Entity final
	{
	public:
		unsigned int id{};
		std::string name{};
		bool enabled{ true };


		// Entities
		Entity* parent{};
		std::list<std::shared_ptr<Entity>> children{};

		// Components
		std::unordered_map<ComponentType, std::shared_ptr<Component>> components{};


		// constructor, expects just a name
		Entity(const std::string& _name);


		EntityType getType();
		std::string getTypeName();
		std::string getTypeNameEx();


		// fast helpers
		Transform getTransform();
		void setTransform(const engine::Transform& transform);
		
		glm::mat4& getWorldTransform();
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

		engine::AABB* getBoundingVolume();

		void setEnabled(bool _enabled);

#pragma endregion



#pragma region Components

		template<typename T, typename... Args>
			requires std::derived_from<T, ComponentBase<T>>
		T* addComponent(Args&&... args) {
			auto component = std::make_shared<T>(std::forward<Args>(args)...);
			ComponentType typeID = T::getStaticTypeID(); // Static call
			components[typeID] = component;
			return static_cast<T*>(component.get());
		}

		template<typename T>
			requires std::derived_from<T, ComponentBase<T>>
		std::shared_ptr<T> getComponent() {
			ComponentType typeID = T::getStaticTypeID(); // Static call
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

		//SphereVolume generateSphereBV(const Model& model);

		
	};
}