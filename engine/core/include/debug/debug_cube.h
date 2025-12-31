#pragma once

#include "../shader.h"
#include "../materials/material.h"
#include "../primitives/primitive.h"

namespace engine
{
    class DebugCube final
    {
    public:
        DebugCube(const glm::vec3& _position = glm::vec3());
        DebugCube(const float& _size, const glm::vec3& _position = glm::vec3());
        DebugCube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position = glm::vec3());
        ~DebugCube() = default;

        void setup();

        std::vector<engine::Vertex> generateCuboidVertices(float width, float height, float depth, float uvScale);


        std::vector<Vertex> generateVertices();

        // draws the model, and thus all its meshes
        void draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform);

        void clean();

    private:
        glm::vec3 m_position{};

        unsigned int m_VBO{}, m_VAO{};

        float m_width{ 1.0f };
        float m_height{ 1.0f };
        float m_depth{ 1.0f };

		static Shader m_shader; // Shared across all instances

        void geometrySetup();
    };
}