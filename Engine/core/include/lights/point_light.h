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

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
                {"intensity", EditorProperty { "Intensity", getIntensity(), editable, 0.0f, 1000.0f, 1.0f, "%.3f" }},
                {"ambientColor", EditorProperty { "Ambient color", getAmbientColor(), editable, 0.0f, 0.0f, 0.0f, "" }},
                {"diffuseColor", EditorProperty { "Diffuse color", getDiffuseColor(), editable, 0.0f, 0.0f, 0.0f, "" }},
                {"specularColor", EditorProperty { "Specular color", getSpecularColor(), editable, 0.0f, 0.0f, 0.0f, "" }}
            };
        }
        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
                { "intensity", [this](EditorPropertyValue value) { getIntensity() = *(std::get_if<float>(&value)); } },
                { "ambientColor", [this](EditorPropertyValue value) { getAmbientColor() = *(std::get_if<Color>(&value)); } },
                { "diffuseColor", [this](EditorPropertyValue value) { getDiffuseColor() = *(std::get_if<Color>(&value)); } },
                { "specularColor", [this](EditorPropertyValue value) { getSpecularColor() = *(std::get_if<Color>(&value)); } }
            };
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) override;

        void clean() override;

    private:
        Sphere m_debug_sphere{};

        void setup() override;
    };
}
