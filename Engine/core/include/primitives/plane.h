#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

#include "../debug/debug_draw_line.h"

namespace engine
{
    class Plane final : public Primitive
    {
    public:
        Plane(const glm::vec3& _position = glm::vec3());
        ~Plane() = default;

        void setup() override;
        void setup(const std::shared_ptr<Material>& material) override;
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) override;

        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;

        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::plane;
        }

        void clean() override;

    private:
        void geometrySetup();

        void drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);

        DebugDraw m_debugDrawLine{};
    };
}
