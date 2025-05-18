#pragma once

#include "../common_defines.h"
#include "../entity.h"

namespace engine
{
	class EntityManager
	{
	public:
		EntityManager() = default;
		~EntityManager() = default;

		Entity& find(const std::string& name);

	};
}

