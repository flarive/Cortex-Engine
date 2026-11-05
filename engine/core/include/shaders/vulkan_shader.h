#pragma once

#include "shader.h"

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <string>


namespace engine
{
    class VulkanShader final : public ShaderBase
    {
    public:
        unsigned int ID{};
        std::string name{};

        VulkanShader() = default;
        ~VulkanShader() override;

        void init(const char* shaderName, const char* vertexPath, const char* fragmentPath) override;
        void init(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath) override;
        void init(const char* shaderName, const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* geometryPath, const char* fragmentPath) override;

        bool isValid() const override;



        bool isInitialized() const { return m_initialized; }

        // activate the shader
        void use() const override;

        bool checkShaderUniformExists(unsigned int shaderID, const std::string& uniformName) const override;

        // utility uniform functions
        void setBool(const std::string& name, bool value) const override;
        void setInt(const std::string& name, int value) const override;
        void setFloat(const std::string& name, float value) const override;
        void setVec2(const std::string& name, const glm::vec2& value) const override;
        void setVec2(const std::string& name, float x, float y) const override;
        void setVec3(const std::string& name, const glm::vec3& value) const override;
        void setVec3(const std::string& name, float x, float y, float z) const override;
        void setVec4(const std::string& name, const glm::vec4& value) const override;
        void setVec4(const std::string& name, float x, float y, float z, float w) override;
        void setMat2(const std::string& name, const glm::mat2& mat) const override;
        void setMat3(const std::string& name, const glm::mat3& mat) const override;
        void setMat4(const std::string& name, const glm::mat4& mat) const override;
        void clean() override;

        void getActiveUniformsList(const std::string& uniformName) override;


    private:
        bool m_initialized = false;

        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(unsigned int shader, std::string type) override;
    };
}
