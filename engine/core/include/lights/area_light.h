#pragma once

#include "light.h"


#include "../debug/debug_draw_line.h"

namespace engine
{
    // Implementing Areal Lights with Linearly Transformed Cosines.
    //
    // Inspiration:
    // https://advances.realtimerendering.com/s2016/s2016_ltc_rnd.pdf
    // https://eheitzresearch.wordpress.com/415-2/
    class AreaLight final : public Light
    {
    public:
        glm::vec3 color{};
        float roughness{ 0.5f };
        bool twoSided{ true };
        
        AreaLight();
        AreaLight(glm::vec3 _position);
        AreaLight(const std::shared_ptr<engine::Primitive>& primitive, glm::vec3 _position = glm::vec3(0.0f));

        LightType getTypeID() const override
        {
            return LightType::area;
        }

        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform) override;

        void clean() override;

    private:

        GLuint areaLightVBO{}, areaLightVAO{};

        Shader shaderLightPlane{};

        DebugDraw m_debugDrawLine{};

		std::shared_ptr<engine::Primitive> m_primitive{};

        VertexAL areaLightVertices[6] = {
            { {-8.0f, 2.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} }, // 0 1 5 4
            { {-8.0f, 2.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f} },
            { {-8.0f, 0.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
            { {-8.0f, 2.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} },
            { {-8.0f, 0.4f,  1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f} },
            { {-8.0f, 0.4f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f} }
        };

        void setup() override;

        void drawDebugNormals(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);
    };
}
