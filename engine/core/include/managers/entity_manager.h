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
		//std::shared_ptr<Entity>& get();

		Entity& find(const std::string& name);


		
	};
}