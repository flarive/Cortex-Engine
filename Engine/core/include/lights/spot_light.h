#pragma once

#include "light.h"

namespace engine
{
    class SpotLight : public Light
    {
    public:
        SpotLight();

        SpotLight(unsigned int index);


        void setup(const Color& ambient = Color(0.01f), const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& target = glm::vec3(0.0f)) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target) override;


        void setCutOff(float cutoff);

        void setOuterCutOff(float outerCutoff);

        void clean() override;

    protected:
        float m_cutoff = 12.5f;
        float m_outerCutoff = 15.0f;
    };
}
