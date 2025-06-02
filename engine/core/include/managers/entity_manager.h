#pragma once

#include "../common_defines.h"
#include "../entity.h"


#include <unordered_map>
#include <typeindex>
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

		std::shared_ptr<engine::Entity> find(const std::string& name);



		// Add this to store components
		//std::unordered_map<std::type_index, std::shared_ptr<void>> m_components;

		//// Add component
		//template<typename T>
		//void addComponent(const std::shared_ptr<T>& component) {
		//	m_components[typeid(T)] = component;
		//}

		// Get component
		/*template<typename T>
		std::shared_ptr<T> getComponent() const {
			auto it = m_entityCache.find(typeid(T));
			if (it != m_entityCache.end()) {
				return std::static_pointer_cast<T>(it->second);
			}
			return nullptr;
		}





		template<typename T>
		std::vector<std::shared_ptr<T>> findComponents()
		{
			std::vector<std::shared_ptr<T>> components;
			findComponentsRecursive<T>(m_rootEntity, components);
			return components;
		};

		template<typename T>
		void findComponentsRecursive(const std::shared_ptr<engine::Entity>& entity, std::vector<std::shared_ptr<T>>& components)
		{
			if (!entity) return;

			auto component = entity->getComponent<T>();
			if (component)
			{
				components.push_back(component);
			}

			for (const auto& child : entity->children)
			{
				findComponentsRecursive<T>(child, components);
			}
		};*/

		void updateSelfAndChild();

	private:
		std::shared_ptr<Entity> m_rootEntity{};
		
		std::unordered_map<std::string, std::shared_ptr<Entity>> m_entityCache{};
		
		std::shared_ptr<engine::Entity> findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name);

		bool removeRecursive(const std::shared_ptr<Entity>& parent, const std::shared_ptr<Entity>& current, const std::string& targetName);

		void addToCache(const std::shared_ptr<Entity>& entity);
		void removeFromCache(const std::shared_ptr<Entity>& entity);
		void removeFromCacheRecursive(const std::shared_ptr<Entity>& entity);
	};
}