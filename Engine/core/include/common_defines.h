#pragma once

#include <glad/glad.h>
#include <glfw3.h> // Will drag system OpenGL headers

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

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
