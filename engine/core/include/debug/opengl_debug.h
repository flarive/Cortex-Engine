#pragma once

#include "../../include/common_defines.h"

namespace engine
{
	class OpenGLDebug
	{
	public:
		static void checkGLError(const char* label);
		static void GLClearError();
		static void GLCheckError();

		static void APIENTRY debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}
