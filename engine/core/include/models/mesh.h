#pragma once

#include <vector>

#include "../misc/noncopyable.h"

#include "../vertex.h"
#include "../materials/material.h"
#include "../shader.h"

namespace engine
{
	class Mesh final : private NonCopyableButMovable
    {
    public:
        
        // mesh Data
        std::vector<Vertex> vertices{};
        std::vector<unsigned int> indices{};

        // constructor
        Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::shared_ptr<Material> _material);
		~Mesh() = default;

        // override a little NonCopyableButMovable
        Mesh(Mesh&&) = default;

        // render the mesh
        void draw(Shader& shader, const glm::mat4& transformMatrix = glm::mat4(1.0f));

        std::shared_ptr<Material> getMaterial() { return m_material; }

        void clean();
       
    private:
        // render data 
        unsigned int m_VBO{}, m_EBO{}, m_VAO{};

        unsigned int m_indexCount{};

        std::shared_ptr<Material> m_material{};

        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
