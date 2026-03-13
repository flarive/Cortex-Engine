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


        bool getUseAttenuation() const { return m_useAttenuation; }
        void setUseAttenuation(bool use) { m_useAttenuation = use; }

        float getConstantAttenuation() const { return m_constantAttenuation; }
        void setConstantAttenuation(float value) { m_constantAttenuation = value; }

        float getLinearAttenuation() const { return m_linearAttenuation; }
        void setLinearAttenuation(float value) { m_linearAttenuation = value; }

        float getQuadraticAttenuation() const { return m_quadraticAttenuation; }
        void setQuadraticAttenuation(float value) { m_quadraticAttenuation = value; }

    private:
        bool m_useAttenuation{ true };

        // constant: A constant factor.Even if the light is very close, this ensures some base attenuation.
        // Usually 1.0 so the denominator never goes to zero.
        float m_constantAttenuation{ 1.0f };

        // linear: Controls how quickly the light falls off linearly with distance.
        float m_linearAttenuation { 0.09f }; //0.09, 0.045, 0.0014

        // quadratic: Controls how quickly the light falls off with the square of the distance (more realistic for point lights).
        float m_quadraticAttenuation { 0.032f }; // 0.032, 0.0075, 0.000007
        
        Sphere m_debug_sphere{};

        void setup() override;
    };
}
