#pragma once

#include "light.h"

#include <format>

namespace engine
{
    class PointLight final : public Light
    {
    public:
        PointLight();

        PointLight(unsigned int index);

        void setup() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target = glm::vec3(0.0f), const glm::vec3& size = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f)) override;

        void clean() override;
    };
}
