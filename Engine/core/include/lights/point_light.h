#pragma once

#include "light.h"
#include "../primitives/sphere.h"

namespace engine
{
    class PointLight final : public Light
    {
    public:
        PointLight();
        PointLight(glm::vec3 _position);

        LightType getTypeID() const override
        {
            return LightType::point;
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) override;

        void clean() override;

    private:
        Sphere m_debug_sphere{};

        void setup() override;
    };
}
