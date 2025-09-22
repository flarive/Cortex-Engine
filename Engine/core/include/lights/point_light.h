#pragma once

#include "light.h"
#include "../primitives/sphere.h"

#include <format>

namespace engine
{
    class PointLight final : public Light
    {
    public:
        PointLight(unsigned int index);
        PointLight(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int index = 0);

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) override;

        void clean() override;

    private:
        Sphere m_debug_sphere{};

        void setup() override;
    };
}
