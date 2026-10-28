#include "../../include/app/vulkan_app.h"

engine::VulkanApp::VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("VulkanApp {} constructor called", _title);
}

engine::VulkanApp::~VulkanApp()
{
    logger.trace("VulkanApp {} destructor called", m_title);
}
