#include "../../include/shaders/shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../../include/common_defines.h"
#include "../../include/managers/log_manager.h"

#include "../../include/app/gpu_selector.h"

#include "../../include/managers/filesystem_manager.h"

#include <string>
#include <format>
#include <filesystem>
#include <stdexcept>


engine::ShaderBase::ShaderBase(const std::string& name)
    : m_name(name)
{
}

engine::ShaderType engine::ShaderBase::getShaderType()
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

std::string engine::ShaderBase::readTextFile(const std::string& path)
{
 //   auto absFullPath = engine::FileSystemManager::getFullPath(path);
	//if (!std::filesystem::exists(absFullPath))
	//{
	//	throw std::runtime_error(std::format("File does not exist: {}", path));
	//}
    
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error(std::format("Failed to open file: {}", path));
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

engine::ShaderBase::~ShaderBase()
{
}