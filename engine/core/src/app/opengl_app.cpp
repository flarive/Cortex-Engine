#include "../../include/app/opengl_app.h"

engine::OpenGLApp::OpenGLApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("OpenGLApp {} constructor called", _title);
}

engine::OpenGLApp::~OpenGLApp()
{
    logger.trace("OpenGLApp {} destructor called", m_title);
}