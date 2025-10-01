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
        
        SpotLight(unsigned int index);
        SpotLight(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), unsigned int index = 0);

        LightType getTypeID() const override
        {
            return LightType::spot;
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) override;

        void clean() override;

    private:
        Cone m_debug_cone{};

        void setup() override;
    };
}
