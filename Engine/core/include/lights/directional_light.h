#pragma once

#include "light.h"


namespace engine
{
    class DirectionalLight : public Light
    {
    public:
        DirectionalLight();
        DirectionalLight(unsigned int index);

        void setup(const Color& ambient = Color(0.01f), const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& target = glm::vec3(0.0f)) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target = glm::zero<glm::vec3>()) override;

        void clean() override;
    };
}
