#pragma once

#include "../common_defines.h"
#include "../ecs/entity.h"


#include "../ecs/camera_component.h"
#include "../ecs/light_component.h"
#include "../ecs/primitive_component.h"
#include "../ecs/model_component.h"


#include <unordered_map>
#include <memory>


namespace engine
{
	/// <summary>
	/// ECS (Entity Component System)
	/// https://en.wikipedia.org/wiki/Entity_component_system
	/// </summary>
	class EntityManager
	{
	public:
		
		const std::string ROOT_ENTITY_NAME = "Root";
		
		EntityManager() = default;
		~EntityManager() = default;

		void create();

		void set(std::shared_ptr<Entity> rootEntity);

		std::shared_ptr<Entity>& getRootEntity();

		void addChild(std::shared_ptr<engine::Entity> child);
		void addChild(std::shared_ptr<engine::Entity> parent, std::shared_ptr<engine::Entity> child);




		bool remove(const std::string& name);

		std::shared_ptr<engine::Entity> findEntityByName(const std::string& name);





		template<typename T>
		std::vector<std::shared_ptr<T>> findEntitiesOfType()
		{
			std::vector<std::shared_ptr<T>> result;
			findEntitiesOfTypeRecursive<T>(m_rootEntity, result);
			return result;
		}

		
		template<typename T>
		std::shared_ptr<T> findEntityOfType()
		{
			std::vector<std::shared_ptr<T>> result;
			findEntitiesOfTypeRecursive<T>(m_rootEntity, result);

			return result.size() > 0 ? result[0] : nullptr;
		}





		void updateSelfAndChild();

	private:
		std::shared_ptr<Entity> m_rootEntity{};
		
		std::unordered_map<std::string, std::shared_ptr<Entity>> m_entityCache{};
		
		std::shared_ptr<engine::Entity> findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name);

		//template<typename T>
		//void findEntitiesOfTypeRecursive(const std::shared_ptr<engine::Entity>& entity, std::vector<std::shared_ptr<T>>& result)
		//{
		//	if (!entity) return;

		//	// Try casting the entity to the desired type
		//	std::shared_ptr<T> casted{};

		//	if (auto primitiveComponent = entity->getComponent<PrimitiveComponent>())
		//	{
		//		casted = std::reinterpret_pointer_cast<T>(primitiveComponent->getPrimitive());
		//	}
		//	else if (auto modelComponent = entity->getComponent<ModelComponent>())
		//	{
		//		casted = std::reinterpret_pointer_cast<T>(modelComponent->getModel());
		//	}
		//	else if (auto lightComponent = entity->getComponent<LightComponent>())
		//	{
		//		if (typeid(std::dynamic_pointer_cast<T>(lightComponent->getLight())) == typeid(std::shared_ptr<T>))
		//		{
		//			casted = std::reinterpret_pointer_cast<T>(lightComponent->getLight());
		//		}
		//	}
		//	else if (auto cameraComponent = entity->getComponent<CameraComponent>())
		//	{
		//		if (typeid(std::dynamic_pointer_cast<T>(cameraComponent->getCamera())) == typeid(std::shared_ptr<T>))
		//		{
		//			casted = std::reinterpret_pointer_cast<T>(cameraComponent->getCamera());
		//		}
		//	}



		//	if (casted)
		//	{
		//		result.push_back(casted);
		//	}

		//	for (const auto& child : entity->children)
		//	{
		//		findEntitiesOfTypeRecursive<T>(child, result);
		//	}
		//}


		template<typename T>
		void findEntitiesOfTypeRecursive(const std::shared_ptr<engine::Entity>& entity, std::vector<std::shared_ptr<T>>& result)
		{
			if (!entity) return;

			std::shared_ptr<T> casted;

			if (auto lightComponent = entity->getComponent<LightComponent>())
			{
				casted = std::dynamic_pointer_cast<T>(lightComponent->getLight());
			}
			else if (auto cameraComponent = entity->getComponent<CameraComponent>())
			{
				casted = std::dynamic_pointer_cast<T>(cameraComponent->getCamera());
			}
			else if (auto primitiveComponent = entity->getComponent<PrimitiveComponent>())
			{
				casted = std::dynamic_pointer_cast<T>(primitiveComponent->getPrimitive());
			}
			else if (auto modelComponent = entity->getComponent<ModelComponent>())
			{
				casted = std::dynamic_pointer_cast<T>(modelComponent->getModel());
			}

			if (casted)
			{
				result.push_back(casted);
			}

			for (const auto& child : entity->children)
			{
				findEntitiesOfTypeRecursive<T>(child, result);
			}
		}





		bool removeRecursive(const std::shared_ptr<Entity>& parent, const std::shared_ptr<Entity>& current, const std::string& targetName);

		void addToCache(const std::shared_ptr<Entity>& entity);
		void removeFromCache(const std::shared_ptr<Entity>& entity);
		void removeFromCacheRecursive(const std::shared_ptr<Entity>& entity);
	};
}