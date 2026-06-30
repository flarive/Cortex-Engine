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

	using ulong = uint64_t;
	using uint = uint32_t;
	using ushort = uint16_t;
	using ubyte = uint8_t;

	//using long = int64_t;
	//using int = int32_t;
	//using short = int16_t;
	using byte = int8_t;

	using EditorPropertyValue = std::variant<int, unsigned int, std::string, float, bool, glm::vec2, glm::vec3, glm::vec4, std::vector<std::string>>;

	enum EditorPropertyType {
		editable = 1 << 0, // 1
		readonly = 1 << 1, // 2
		label = 1 << 2, // 4
		noheader = 1 << 3, // 8,
		combobox = 1 << 4 // 16
	};

	struct EditorProperty
	{
		std::string display{};
		EditorPropertyValue value{};
		short type{}; // Use EditorPropertyType flags
		float min{};
		float max{};
		float step{};
		std::string format{};
		std::string suffix{};
		std::function<void(unsigned short)> function{};
	};
}
