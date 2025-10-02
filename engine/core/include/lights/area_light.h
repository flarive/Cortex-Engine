#pragma once

#include "light.h"
#include "../primitives/cylinder.h"


namespace engine
{
    class AreaLight final : public Light
    {
    public:
        AreaLight(unsigned int index);
        AreaLight(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int index = 0);


        LightType getTypeID() const override
        {
            return LightType::directional;
        }
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) override;

        void clean() override;

    private:
        Cylinder m_debug_cylinder{};

        void setup() override;
    };
}
