#pragma once

#include "../common_defines.h"
#include "../entity.h"

namespace engine
{
	class EntityManager
	{
	public:
		
		std::shared_ptr<Entity> rootEntity{};
		std::unordered_map<std::string, std::shared_ptr<Entity>> nameCache;
		
		EntityManager() = default;
		~EntityManager() = default;

		void create();

		void set(std::shared_ptr<Entity> rootEntity);

		void addChild(std::shared_ptr<engine::Entity> child);

		void addChild(std::shared_ptr<engine::Entity> parent, std::shared_ptr<engine::Entity> child);

		std::shared_ptr<engine::Entity> find(const std::string& name);

	private:
		std::shared_ptr<engine::Entity> findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name);

		//void buildCache(); // Optional: builds the cache once
		
	};
}