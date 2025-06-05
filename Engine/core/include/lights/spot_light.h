#pragma once

#include "light.h"

namespace engine
{
    class SpotLight final : public Light
    {
    public:
        SpotLight();

        SpotLight(unsigned int index);


        void setup() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f)) override;


        void setCutOff(float cutoff);

        void setOuterCutOff(float outerCutoff);

        void clean() override;

    protected:
        float m_cutoff = 12.5f;
        float m_outerCutoff = 15.0f;
    };
}
