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

        void setup(const Color& ambient = Color(0.01f), const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& target = glm::vec3(0.0f)) override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity) override;

        void clean() override;
    };
}
