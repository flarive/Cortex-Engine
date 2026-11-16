#include "../../include/shaders/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../../include/common_defines.h"
#include "../../include/singleton.h"

#include "../../include/managers/log_manager.h"

#include "../../include/app/gpu_selector.h"

#include "../../include/managers/filesystem_manager.h"

#include "../../include/shaders/opengl_shader.h"
#include "../../include/shaders/vulkan_shader.h"

#include <string>
#include <format>
#include <filesystem>
#include <stdexcept>


engine::Shader::Shader(const std::string& name)
    : m_name(name)
{
}

std::unique_ptr<engine::Shader> engine::Shader::create(const std::string& name)
{
    const AppSettings& settings = engine::Singleton::getInstance()->appSettings();
    if (settings.backend == BackendType::Vulkan)
    {
        return std::make_unique<engine::VulkanShader>(name);
    }
    
    return std::make_unique<engine::OpenGLShader>(name);
}

engine::ShaderType engine::Shader::getShaderType()
{
    if (m_name == "pbr") return ShaderType::PBR;
    if (m_name == "blinnphong") return ShaderType::BlinnPhong;
    if (m_name == "phong") return ShaderType::Phong;
    
    if (m_name == "simpleDepthBuffer1") return ShaderType::DepthBufferDirectionalLights;
    if (m_name == "simpleDepthBuffer2") return ShaderType::DepthBufferPointLights;

    if (m_name == "pbrTessellation") return ShaderType::PBRTessellation;
    if (m_name == "blinnphongTessellation") return ShaderType::BlinnPhongTessellation;

    if (m_name == "simpleDepthBuffer1Tess") return ShaderType::DepthBufferDirectionalLightsTessellation;
    if (m_name == "simpleDepthBuffer2Tess") return ShaderType::DepthBufferPointLightsTessellation;

    return ShaderType::Unknown;
}

engine::Shader::~Shader()
{
}