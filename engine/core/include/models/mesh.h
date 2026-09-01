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
        // constructor
        Mesh(const std::string& _name, std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::shared_ptr<Material> _material);
		~Mesh();

        // override a little NonCopyableButMovable
        Mesh(Mesh&&) = default;

        // render the mesh
        void draw(Shader& shader, const glm::mat4& transformMatrix = glm::mat4(1.0f));

        std::shared_ptr<Material> getMaterial() { return m_material; }

		const std::string& getName() const { return m_name; }

        const std::vector<Vertex>& getVertices() const { return m_vertices; }
        const std::vector<unsigned int>& getIndices() const { return m_indices; }


        // Bind-pose fallback
        std::vector<glm::mat4> bindPoseMatrices{};
        bool hasBones = false;
        bool hasAnimations = false;


        void clean();
       
    private:

        std::string m_name{};

        // mesh Data
        std::vector<Vertex> m_vertices{};
        std::vector<unsigned int> m_indices{};

        // render data 
        unsigned int m_VBO{}, m_EBO{}, m_VAO{};

        unsigned int m_indexCount{};

        std::shared_ptr<Material> m_material{};







        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
