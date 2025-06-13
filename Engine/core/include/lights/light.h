#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../shader.h"
#include "../primitives/primitive.h"

namespace engine
{
    /// <summary>
    /// Abstract class for lights
    /// </summary>
    class Light : private NonCopyable
    {
    public:
        Light();
        Light(unsigned int index);
        virtual ~Light() = default;

        virtual void setup() = 0;
        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size, const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f)) = 0;
        virtual void clean() = 0;
        
        virtual glm::vec3 getPosition() const { return m_lightPosition; }
        virtual glm::vec3 getTarget() const { return m_lightTarget; }

        virtual float getIntensity() const { return m_intensity; }
        virtual Color getAmbientColor() const { return m_ambientColor; }

        virtual void setPosition(const glm::vec3& position) { m_lightPosition = position; }
        virtual void setTarget(const glm::vec3& target) { m_lightTarget = target; }

        virtual void setIntensity(const float intensity);
        virtual void setAmbientColor(const engine::Color& ambientColor);

    protected:
        // render data 
        unsigned int VBO{}, VAO{};

        unsigned int m_index{};

        Shader m_lightCubeShader{};

        Color m_ambientColor{ glm::vec4(0.1f, 0.1f, 0.1f, 1.0f) };

        glm::vec3 m_lightPosition{};
        glm::vec3 m_lightTarget{};

        float m_intensity{ 1.0f };

        const float LIGHT_CUBE_SIZE{ 0.02f };
        const bool DISPLAY_DEBUG_LIGHT_CUBE{ false };

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
