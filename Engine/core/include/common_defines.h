#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h> // Will drag system OpenGL headers

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>


// Cross-platform UNREFERENCED_PARAMETER macro
//#ifndef UNREFERENCED_PARAMETER
//#define UNREFERENCED_PARAMETER(P) (void)(P)
//#endif

namespace engine
{
	using Color = glm::vec4;
}
