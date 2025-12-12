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

	using EditorPropertyValue = std::variant<int, unsigned int, std::string, float, bool, glm::vec2, glm::vec3, glm::vec4>;

	struct EditorProperty
	{
		EditorPropertyValue value{};
		float min{};
		float max{};
		float step{};
		std::string format{};
		bool readOnly{ false };
	};
}
