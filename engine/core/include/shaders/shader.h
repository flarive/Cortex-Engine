#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <string>
#include <unordered_map>

namespace engine
{
    enum class ShaderType {
        Unknown,
        Phong,
        BlinnPhong,
        BlinnPhongTessellation,
        PBR,
        PBRTessellation,
        DepthBufferDirectionalLights,
        DepthBufferPointLights,
        DepthBufferDirectionalLightsTessellation,
        DepthBufferPointLightsTessellation
    };

    struct UniformBuffer final
    {
        GLuint id = 0;
        GLuint binding = 0;
        size_t size = 0;
    };

    struct DebugLightCameraUBO final
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    struct DebugLightColorUBO final
    {
        glm::vec4 customColor;
    };

    class Shader : private NonCopyable
    {
    public:
        Shader(const std::string& name);
        virtual ~Shader();

        virtual void init(const char* vertexPath, const char* fragmentPath) = 0;
        virtual void init(const char* vertexPath, const char* fragmentPath, const char* geometryPath) = 0;
        virtual void init(const char* vertexPath, const char* tessControlPath, const char* tessEvalPath, const char* geometryPath, const char* fragmentPath) = 0;

        virtual bool isValid() const = 0;



        bool isInitialized() const { return m_initialized; }

        // activate the shader
        virtual void use() const = 0;

        virtual bool checkShaderUniformExists(unsigned int shaderID, const std::string& uniformName) const = 0;

        // utility uniform functions
        virtual void setBool(const std::string& name, bool value) const = 0;
        virtual void setInt(const std::string& name, int value) const = 0;
        virtual void setFloat(const std::string& name, float value) const = 0;
        virtual void setVec2(const std::string& name, const glm::vec2& value) const = 0;
        virtual void setVec2(const std::string& name, float x, float y) const = 0;
        virtual void setVec3(const std::string& name, const glm::vec3& value) const = 0;
        virtual void setVec3(const std::string& name, float x, float y, float z) const = 0;
        virtual void setVec4(const std::string& name, const glm::vec4& value) const = 0;
        virtual void setVec4(const std::string& name, float x, float y, float z, float w) = 0;
        virtual void setMat2(const std::string& name, const glm::mat2& mat) const = 0;
        virtual void setMat3(const std::string& name, const glm::mat3& mat) const = 0;
        virtual void setMat4(const std::string& name, const glm::mat4& mat) const = 0;
        virtual void clean() = 0;

        ShaderType getShaderType();
        virtual void getActiveUniformsList(const std::string& uniformName) = 0;


        const unsigned int getID() { return m_ID; }
        const std::string& getName() { return m_name; }


        static std::unique_ptr<Shader> create(const std::string& name);

        // UBOs
        virtual void createUniformBuffer(const std::string& blockName, GLuint binding, size_t bufferSize) = 0;
        virtual void updateUniformBuffer(const std::string& blockName, const void* data, size_t size) = 0;

    private:
        bool m_initialized = false;

        

        // utility function for checking shader compilation/linking errors.
        // ------------------------------------------------------------------------
        virtual void checkCompileErrors(unsigned int shader, std::string type) = 0;

    protected:
        unsigned int m_ID{};
        std::string m_name{};

        std::unordered_map<std::string, UniformBuffer> m_uniformBuffers;
    };
}