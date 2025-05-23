#pragma once

#include "../common_defines.h"

namespace engine
{
	struct NormalizedRotation
	{
		glm::vec3 axis{};
		float angle{};
	};
	
	
	class Helpers
	{
	public:
		static NormalizedRotation normalizeRotation(glm::vec3 rotation);
	};
}