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

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"Intensity", EditorProperty { getIntensity(), 0.0f, 1000.0f, 1.0f, "%.3f" }},
                //{"Target", EditorProperty { getTarget(), -180.0f, 180.0f, 1.0f, "%.2f" }},
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "intensity", [this](EditorPropertyValue value) { getIntensity() = *(std::get_if<float>(&value)); } }
                //{"target", [this](glm::vec3 value) { getTarget() = value; }},
            };
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) override;


        void clean() override;

    private:
        Cylinder m_debug_cylinder{};
        
        void setup() override;
    };
}
