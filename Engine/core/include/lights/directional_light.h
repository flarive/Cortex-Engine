#pragma once

#include "light.h"
#include "../primitives/cylinder.h"

namespace engine
{
    class DirectionalLight final : public Light
    {
    public:
        DirectionalLight();
        DirectionalLight(glm::vec3 _position);


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
