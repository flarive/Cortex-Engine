#pragma once

#pragma warning(disable : 4005) // avoid warning C4005: 'APIENTRY' : redefinition de macro

#include <glad/glad.h>
#include <glfw/glfw3.h> // Will drag system OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <functional>

#define EDITOR_MODE true


namespace engine
{
	using Color = glm::vec4;

	using EditorPropertyValue = std::variant<int, unsigned int, std::string, float, bool, glm::vec2, glm::vec3, glm::vec4, std::vector<std::string>>;

	struct EditorProperty
	{
		std::string displayName{};
		EditorPropertyValue value{};
		bool readOnly{ false };
		float min{};
		float max{};
		float step{};
		std::string format{};
		std::function<void(unsigned short)> function{};
	};
}
