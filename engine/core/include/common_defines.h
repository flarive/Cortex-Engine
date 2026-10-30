#pragma once

#pragma warning(disable : 4005) // avoid warning C4005: 'APIENTRY' : redefinition de macro


// include GLFW window system lib
// tell GLFW not to include GLAD itself
//#define GLFW_INCLUDE_NONE
//#include <glfw/glfw3.h>
//
//// include GLAD lib
//#include <opengl/glad.h> // openGL
//#include <vulkan/vulkan.h> // Vulkan


#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <opengl/glad.h>


// include imGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h> // openGL imGUI backend
#include <backends/imgui_impl_vulkan.h> // vulkan imGUI backend
#include <themes/imgui_spectrum.h>

// include GLM math lib
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include common standard library stuffs
#include <string>
#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <functional>

//Project Properties → C / C++ → Preprocessor → Preprocessor Definitions
//#define EDITOR_MODE 0




#ifndef EDITOR_MODE
#define EDITOR_MODE 0
#endif

//#define STRINGIFY2(x) #x
//#define STRINGIFY(x) STRINGIFY2(x)

//#ifdef EDITOR_MODE
//#pragma message("EDITOR_MODE defined")
//#endif
//
//#if EDITOR_MODE
//#pragma message("EDITOR_MODE true")
//#else
//#pragma message("EDITOR_MODE false")
//#endif


namespace engine
{
	using Color = glm::vec4;

	using ulong = uint64_t;   // 0 to 18,446,744,073,709,551,615  (2^64 - 1)
	using uint = uint32_t;    // 0 to 4,294,967,295               (2^32 - 1)
	using ushort = uint16_t;  // 0 to 65,535                      (2^16 - 1)
	using ubyte = uint8_t;    // 0 to 255                         (2^8  - 1)


	//using long = int64_t;
	//using int = int32_t;
	//using short = int16_t;
	using byte = int8_t;

	using EditorPropertyValue = std::variant<int, unsigned int, std::string, float, bool, glm::vec2, glm::vec3, glm::vec4, std::vector<std::string>>;

	enum EditorPropertyType {
		editable = 1 << 0, // 1
		readonly = 1 << 1, // 2
		label = 1 << 2, // 4
		noheader = 1 << 3, // 8
		combobox = 1 << 4, // 16
		slider = 1 << 5, // 32
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

	const float DEBUG_DRAW_NORMAL_LENGTH = 1.0f;
	const float DEBUG_DRAW_NORMAL_ARROW_SIZE = 0.25f;
	const glm::vec3 DEBUG_DRAW_NORMAL_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
	const bool DEBUG_DRAW_NORMAL_DRAW_ARROW = true;

	struct PerformanceCounters
	{
		float fps;
		float deltaTime;
		double cpuTime;
		double gpuTime;
		double uiTime;
	};
}
