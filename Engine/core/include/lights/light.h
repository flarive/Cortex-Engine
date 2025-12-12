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

    enum class ShadowCalculationMethod : int {
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
        
        float intensity{ 1.0f };
        Color ambientColor{ Color(0.1f, 0.1f, 0.1f, 1.0f) };
        Color diffuseColor{ Color(1.0f) }; // ?????
        Color specularColor{ Color(1.0f) }; // ?????


        glm::vec3 position{};
        glm::vec3 scale{};
        glm::vec3 rotation{};

        glm::vec3 target{};
        
        Light();
        Light(glm::vec3 _position);
        virtual ~Light() = default;

        virtual LightType getTypeID() const
        {
            return LightType::undefined;
        }

        float& getIntensity() { return intensity; }
        void setIntensity(float _intensity) { intensity = _intensity; }

        Color& getAmbientColor() { return ambientColor; }
        void setAmbientColor(Color _color) { ambientColor = _color; }

        Color& getDiffuseColor() { return diffuseColor; }
        void setDiffuseColor(Color _color) { diffuseColor = _color; }

        Color& getSpecularColor() { return specularColor; }
        void setSpecularColor(Color _color) { specularColor = _color; }

        glm::vec3& getTarget() { return target; }
        void setTarget(glm::vec3 _target) { target = _target; }


        virtual void setIndex(unsigned int index) { m_index = index; }

        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) = 0;

        virtual void clean() = 0;

        void setEnabled(bool _enabled) { m_enabled = _enabled; }

        virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;



    private:
        virtual void setup() = 0;

    protected:
        unsigned int m_index{};

		bool m_enabled{ true };

        Shader m_lightDebugShader{};

        const float LIGHT_CUBE_SIZE{ 0.82f };
        const bool DISPLAY_DEBUG_LIGHT{ false };

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
