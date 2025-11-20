#pragma once

#pragma warning(disable : 4005) // avoid warning C4005: 'APIENTRY' : redefinition de macro

#include <glad/glad.h>
#include <glfw/glfw3.h> // Will drag system OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>
#include <variant>

#define EDITOR_MODE true


namespace engine
{
	using Color = glm::vec4;

	struct KeyValuePair
	{
		std::string key{};
		std::variant<int, std::string, float, bool> value{};
	};;
}
