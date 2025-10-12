#pragma once

#include "light.h"
#include "../primitives/cone.h"

namespace engine
{
    class SpotLight final : public Light
    {
    public:
        float cutoff = 12.5f;
        float outerCutoff = 15.0f;
        
        SpotLight();
        SpotLight(glm::vec3 _position);

        LightType getTypeID() const override
        {
            return LightType::spot;
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) override;

        void clean() override;

    private:
        Cone m_debug_cone{};

        void setup() override;
    };
}
