#pragma once

#include "light.h"

namespace engine
{
    class AreaLight final : public Light
    {
    public:
        glm::vec3 offset;
        float yRotation;

        glm::vec3 color;
        float intensity = 4.0f;
        bool twoSided = true;
        
        AreaLight(unsigned int index);
        AreaLight(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int index = 0);


        LightType getTypeID() const override
        {
            return LightType::directional;
        }
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) override;

        void clean() override;

    private:

        GLuint areaLightVBO, areaLightVAO;

        // SHADERS
        engine::Shader shaderLightPlane;


        VertexAL areaLightVertices[6] = {
    { {-8.0f, 2.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 0 1 5 4
    { {-8.0f, 2.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
    { {-8.0f, 0.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
    { {-8.0f, 2.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
    { {-8.0f, 0.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
    { {-8.0f, 0.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} }
        };

        void setup() override;
    };
}
