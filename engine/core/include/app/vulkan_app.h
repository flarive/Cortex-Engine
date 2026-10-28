#pragma once

#include "app.h"

namespace engine
{
	class VulkanApp : public App
	{
		VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
		virtual ~VulkanApp();
	};
}