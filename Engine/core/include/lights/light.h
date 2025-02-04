#pragma once

#include "../common_defines.h"
#include "../shader.h"
#include "../primitives/primitive.h"

namespace engine
{
    /// <summary>
    /// Abstract class for lights
    /// </summary>
    class Light
    {
    public:
        Light();
        Light(unsigned int index);
        virtual ~Light() = default;

        virtual void setup() = 0;
        virtual void draw(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target = glm::vec3(0, 0, 0)) = 0;
        virtual void clean() = 0;
        
        virtual glm::vec3 getPosition() { return m_lightPosition; }

    protected:
        // render data 
        unsigned int VBO = 0, VAO = 0;

        unsigned int m_index = 0;

        Shader lightCubeShader;

        glm::vec3 m_lightPosition{};

        const float LIGHT_CUBE_SIZE = 0.02f;
        const bool DISPLAY_DEBUG_LIGHT_CUBE = false;

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
