#pragma once

#include "../common_defines.h"
#include "../entity.h"

namespace engine
{
	class EntityManager
	{
	public:
		
		std::shared_ptr<Entity> rootEntity{};
		
		EntityManager() = default;
		~EntityManager() = default;

		void create();

		void set(std::shared_ptr<Entity> rootEntity);

		std::shared_ptr<engine::Entity> find(const std::string& name);

	private:
		std::shared_ptr<engine::Entity> findEntityRecursive(const std::shared_ptr<engine::Entity>& entity, const std::string& name);
		
	};
}