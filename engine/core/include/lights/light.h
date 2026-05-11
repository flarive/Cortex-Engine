#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../shader.h"
#include "../primitives/primitive.h"

#include "../misc/ordered_map.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include <cstdint> // For int8_t, uint8_t, etc.

namespace engine
{
    enum class LightType { undefined = 0, point = 1, directional = 2, spot = 3, area = 4 };

    const std::unordered_map<LightType, std::string> LightTypeNames = {
        {LightType::undefined, "undefined"},
        {LightType::point, "Point light"},
        {LightType::directional, "Directional light"},
        {LightType::spot, "Spot light"},
        {LightType::area, "Area light"}
    };

    inline std::string to_string(LightType type) {
        auto it = LightTypeNames.find(type);
        return it != LightTypeNames.end() ? it->second : "unknown";
    }

	// simple enum to send to shader for shadow calculation method selection
    enum ShadowCalculationMethod {
        Unknown,
        PCF,
        PCFSoft,
        PCSS
    };
    
    /// <summary>
    /// Abstract class for lights
    /// </summary>
    class Light : private NonCopyable
    {
    public:
        Light();
        Light(glm::vec3 _position);
        virtual ~Light();

        virtual LightType getTypeID() const
        {
            return LightType::undefined;
        }

        glm::vec3& getPosition() { return m_position; }
        glm::vec3& getRotation() { return m_rotation; }
        glm::vec3& getScale() { return m_scale; }

        void setPosition(const glm::vec3& position) { m_position = position; }
        void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
        void setScale(const glm::vec3& scale) { m_scale = scale; }

        void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            m_position = position;
            m_rotation = rotation;
            m_scale = scale;
        }

        float& getIntensity() { return m_intensity; }
        void setIntensity(float _intensity) { m_intensity = _intensity; }

        Color& getAmbientColor() { return m_ambientColor; }
        void setAmbientColor(Color _color) { m_ambientColor = _color; }

        Color& getDiffuseColor() { return m_diffuseColor; }
        void setDiffuseColor(Color _color) { m_diffuseColor = _color; }

        Color& getSpecularColor() { return m_specularColor; }
        void setSpecularColor(Color _color) { m_specularColor = _color; }

        glm::vec3& getTarget() { return m_target; }
        void setTarget(glm::vec3 _target) { m_target = _target; }


        virtual void setIndex(unsigned int index) { m_index = index; }

        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) = 0;

        virtual void clean() = 0;

        virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

		bool getEnabled() const { return m_enabled; }
        void setEnabled(bool enabled) { m_enabled = enabled; }

        

    private:
        virtual void setup() = 0;

    protected:
        unsigned int m_index{};

        bool m_enabled{ true };

        glm::vec3 m_position{};
        glm::vec3 m_scale{};
        glm::vec3 m_rotation{};

        float m_intensity{ 1.0f };

        Color m_ambientColor{ Color(0.1f, 0.1f, 0.1f, 1.0f) };
        Color m_diffuseColor{ Color(1.0f) }; // ?????
        Color m_specularColor{ Color(1.0f) }; // ?????

        glm::vec3 m_target{};

        Shader m_lightDebugShader{};

        const float LIGHT_CUBE_SIZE{ 0.82f };
        const bool DISPLAY_DEBUG_LIGHT{ false };

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
