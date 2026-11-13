#include "../../include/shaders/vulkan_shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../../include/common_defines.h"
#include "../../include/managers/log_manager.h"

#include "../../include/managers/filesystem_manager.h"

#include "../../include/app/vulkan_globals.h"

#include <string>
#include <format>
#include <filesystem>
#include <stdexcept>


engine::VulkanShader::VulkanShader(const std::string& name) : ShaderBase(name)
{
}

/// <summary>
/// Vulkan simple pipeline
/// https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
/// VS => FS
/// </summary>
void engine::VulkanShader::init(const char* vertexPath, const char* fragmentPath)
{
    std::string vertSpvPath = std::string(vertexPath) + ".spv";
    std::string fragSpvPath = std::string(fragmentPath) + ".spv";
    
    // 1. retrieve the vertex/fragment source code from filePath
    std::vector<char> vertShaderCode{};
    std::vector<char> fragShaderCode{};

    try
    {
        vertShaderCode = readBinaryFile(vertSpvPath);
        fragShaderCode = readBinaryFile(fragSpvPath);
    }
    catch (const std::exception& e)
    {
        logger.error("Shader {}: {}", m_name, e.what());
        throw;
    }

    // Creating shader modules
    VkShaderModule vertShaderModule = createShaderModule(engine::g_Device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(engine::g_Device, fragShaderCode);

    // Shader stage creation for vertex shader
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    // Shader stage creation for fragment shader
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


    vkDestroyShaderModule(engine::g_Device, fragShaderModule, nullptr);
    vkDestroyShaderModule(engine::g_Device, vertShaderModule, nullptr);
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

std::vector<char> engine::VulkanShader::readBinaryFile(const std::string& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


VkShaderModule engine::VulkanShader::createShaderModule(const VkDevice& device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());


    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

engine::VulkanShader::~VulkanShader()
{

}

