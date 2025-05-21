#pragma once

#include "../common_defines.h"
#include "../entity.h"

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