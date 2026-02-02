#pragma once
//#include <glm/glm.hpp>
//#include <glm/ext.hpp>

#include "../common_defines.h"

#include <stdlib.h>   
#include <time.h>  

namespace engine
{
	class Particle
	{
	public:
		Particle();

		glm::vec3 position{};
		glm::vec3  startVel{};
		double lifeSpan{};
	};
}

