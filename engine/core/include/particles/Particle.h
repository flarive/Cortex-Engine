#pragma once

#include "../common_defines.h"

#include <stdlib.h>   
#include <time.h>  

namespace engine
{
	class Particle final
	{
	public:
		Particle();

		glm::vec3 position{};
		glm::vec3  startVel{};
		double lifeSpan{};
	};
}

