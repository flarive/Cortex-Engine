#pragma once

#include "../common_defines.h"

#pragma warning(disable : 4324)

//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

namespace engine
{
	struct DebugLightCameraUBO final
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct DebugLightColorUBO final
	{
		glm::vec4 customColor;
	};


    // Phong
	struct PhongCameraUBO final
	{
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct PhongModelUBO final
	{
		glm::mat4 model;
	};

	struct alignas(16) PhongUBO final
	{
		glm::vec4 lightPos;
		glm::vec4 viewPos;
		glm::vec4 lightColor;
		glm::vec4 objectColor;

		uint32_t hasDiffuseTexture;
	};
}