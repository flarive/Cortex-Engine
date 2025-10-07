#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../shader.h"
#include "../primitives/primitive.h"

namespace engine
{
    enum class LightType { undefined = 0, point = 1, directional = 2, spot = 3, area = 4 };
    
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
        
        Light(unsigned int index);
        Light(glm::vec3 _position, unsigned int index);
        virtual ~Light() = default;

        virtual LightType getTypeID() const
        {
            return LightType::undefined;
        }

        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix) = 0;

        virtual void clean() = 0;



    private:
        virtual void setup() = 0;

    protected:
        unsigned int m_index{};

        Shader m_lightDebugShader{};

        const float LIGHT_CUBE_SIZE{ 0.82f };
        const bool DISPLAY_DEBUG_LIGHT{ false };

        glm::vec3 calculateLightDirection(const glm::vec3& position, const glm::vec3& target);
    };
}
