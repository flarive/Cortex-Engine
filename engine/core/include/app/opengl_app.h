#pragma once

#include "app.h"

namespace engine
{
	class OpenGLApp : public App
	{
	public:
		OpenGLApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
		~OpenGLApp() override;

		void start() override;
		void exit() override;

	private:
		void setup() override;

		void initGLFW() override;
		const char* initBackend() override;
		void initImGUI(const char* backend_version) override;

		void enableVerticalSync(bool enable) override;
		void enableMouseCapture(bool enable) override;

		//static void glfw_error_callback(int error, const char* description);
		static void GLAPIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	};
}