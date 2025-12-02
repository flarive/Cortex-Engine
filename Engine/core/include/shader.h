#pragma once

#include "misc/noncopyable.h"
#include "common_defines.h"

#include <string>


namespace engine
{
    enum class ShaderType {
        Unknown,
        BlinnPhong,
        PBR,
        AnimModel
    };

    

    class Shader final : private NonCopyable
    {
    public:
        unsigned int ID{};
        std::string name{};

        Shader() = default;
        ~Shader() = default;

        void init(const char* shaderName, const char* vertexPath, const char* fragmentPath);
        void init(const char* shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath);

        bool isValid() const;



        bool isInitialized() const { return m_initialized; }

        // activate the shader
        void use();

        bool checkShaderUniformExists(unsigned int shaderID, std::string uniformName);

        // utility uniform functions
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w);
        void setMat2(const std::string& name, const glm::mat2& mat) const;
        void setMat3(const std::string& name, const glm::mat3& mat) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;
        void clean();

        ShaderType getShaderType();


    private:
        bool m_initialized = false;

        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}
