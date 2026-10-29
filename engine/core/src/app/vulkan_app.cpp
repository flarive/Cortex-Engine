#include "../../include/app/vulkan_app.h"

engine::VulkanApp::VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("VulkanApp {} constructor called", _title);

    setup();
}

void engine::VulkanApp::start()
{

}

void engine::VulkanApp::setup()
{
}

void engine::VulkanApp::initGLFW()
{
}

const char* engine::VulkanApp::initBackend()
{
    return nullptr;
}

void engine::VulkanApp::initWindow()
{
}

void engine::VulkanApp::initGLAD()
{
}

void engine::VulkanApp::initImGUI(const char* glsl_version)
{
}

void engine::VulkanApp::enableVerticalSync(bool enable)
{
}

void engine::VulkanApp::enableMouseCapture(bool enable)
{
}

void engine::VulkanApp::glfw_error_callback(int error, const char* description)
{
    
}

void GLAPIENTRY engine::VulkanApp::vulkanDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
}

void engine::VulkanApp::exit()
{

}

engine::VulkanApp::~VulkanApp()
{
    logger.trace("VulkanApp {} destructor called", m_title);
}
