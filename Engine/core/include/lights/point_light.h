#pragma once

#include "light.h"

#include <format>

namespace engine
{
    class PointLight : public Light
    {
    public:
        PointLight();

        PointLight(unsigned int index);

        void setup(const Color& ambient = Color(0.01f)) override;

        // draws the model, and thus all its meshes
        void draw(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target = glm::zero<glm::vec3>()) override;

        void clean() override;
    };
}
