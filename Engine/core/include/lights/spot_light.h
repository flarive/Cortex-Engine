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

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"target", EditorProperty { "Target", getTarget(), editable, -180.0f, 180.0f, 1.0f, "%.2f" }},
                {"intensity", EditorProperty { "Intensity", getIntensity(), editable, 0.0f, 1000.0f, 1.0f, "%.3f" }},
                {"cutoff", EditorProperty { "Inner cutoff", getCutoff(), editable, 0.0f, 1000.0f, 1.0f, "%.3f" }},
                {"outerCutoff", EditorProperty { "Outer cutoff", getOuterCutoff(), editable, 0.0f, 1000.0f, 1.0f, "%.3f" }}
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "intensity", [this](EditorPropertyValue value) { getIntensity() = *(std::get_if<float>(&value)); } },
                { "target", [this](EditorPropertyValue value) { getTarget() = *(std::get_if<glm::vec3>(&value)); } },
                { "cutoff", [this](EditorPropertyValue value) { getCutoff() = *(std::get_if<float>(&value)); } },
                { "outerCutoff", [this](EditorPropertyValue value) { getOuterCutoff() = *(std::get_if<float>(&value)); } }
            };
        }

        float& getCutoff() { return cutoff; }
        void setCutoff(float _cutoff) { cutoff = _cutoff; }

        float& getOuterCutoff() { return outerCutoff; }
        void setOuterCutoff(float _outerCutoff) { outerCutoff = _outerCutoff; }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) override;

        void clean() override;

    private:
        Cone m_debug_cone{};

        void setup() override;
    };
}
