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

        virtual void setup(const Color& ambient, const glm::vec3& position, const glm::vec3& target) = 0;
        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target = glm::vec3(0, 0, 0)) = 0;
        virtual void clean() = 0;
        
        virtual glm::vec3 getPosition() { return m_lightPosition; }
        virtual glm::vec3 getTarget() { return m_lightTarget; }

    protected:
        // render data 
        unsigned int VBO{}, VAO{};

        unsigned int m_index{};

        Shader lightCubeShader{};

        Color m_ambientColor{};

        glm::vec3 m_lightPosition{};
        glm::vec3 m_lightTarget{};

        const float LIGHT_CUBE_SIZE{ 0.02f };
        const bool DISPLAY_DEBUG_LIGHT_CUBE{ false };

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
