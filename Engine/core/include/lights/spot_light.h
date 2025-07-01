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
        SpotLight(unsigned int index);

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size = glm::vec3(1.0f), const glm::vec3& rotation = glm::vec3(0.0f)) override;

        void clean() override;

    private:
        Cone m_debug_cone{};

        void setup() override;
    };
}
