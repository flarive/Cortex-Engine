#pragma once

#include "app.h"

namespace engine
{
	class OpenGLApp : public App
	{
	public:
		OpenGLApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
		virtual ~OpenGLApp();
	};
}