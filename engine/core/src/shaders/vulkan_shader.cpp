#include "../../include/shaders/vulkan_shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../../include/common_defines.h"
#include "../../include/managers/log_manager.h"

#include "../../include/managers/filesystem_manager.h"

#include <string>
#include <format>
#include <filesystem>
#include <stdexcept>


engine::VulkanShader::VulkanShader(const std::string& name) : ShaderBase(name)
{
}

/// <summary>
/// OpenGL simple pipeline
/// VS => FS
/// </summary>
void engine::VulkanShader::init(const char* vertexPath, const char* fragmentPath)
{
}

/// <summary>
/// OpenGL pipeline without tessellation
/// VS => (optional GS) => FS
/// </summary>
void engine::VulkanShader::init(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
{
}

/// <summary>
/// OpenGL pipeline with tessellation
/// VS => TCS => TES => (optional GS) => FS
/// </summary>
void engine::VulkanShader::init(const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* geometryPath, const char* fragmentPath)
{
}

bool engine::VulkanShader::isValid() const
{
    return false;
}

// activate the shader
// ------------------------------------------------------------------------
void engine::VulkanShader::use() const
{

}

// check uniform in shader
// -------------------------------------------------------------------------
bool engine::VulkanShader::checkShaderUniformExists(unsigned int shaderID, const std::string& uniformName) const
{
    return true;
}

// utility uniform functions
// ------------------------------------------------------------------------
void engine::VulkanShader::setBool(const std::string& name, bool value) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setInt(const std::string& name, int value) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setFloat(const std::string& name, float value) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setVec2(const std::string& name, const glm::vec2& value) const
{
}
void engine::VulkanShader::setVec2(const std::string& name, float x, float y) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setVec3(const std::string& name, const glm::vec3& value) const
{
}
void engine::VulkanShader::setVec3(const std::string& name, float x, float y, float z) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setVec4(const std::string& name, const glm::vec4& value) const
{
}
void engine::VulkanShader::setVec4(const std::string& name, float x, float y, float z, float w)
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    
}
// ------------------------------------------------------------------------
void engine::VulkanShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
}

void engine::VulkanShader::clean()
{
}

void engine::VulkanShader::checkCompileErrors(unsigned int shader, std::string type)
{
    
}

void engine::VulkanShader::getActiveUniformsList(const std::string& uniformName)
{
    
    
}

engine::VulkanShader::~VulkanShader()
{

}

