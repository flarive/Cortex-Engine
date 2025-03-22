#pragma once

#include <string>
#include <vector>

#include "misc/noncopyable.h"

#include "vertex.h"
#include "materials/material.h"
#include "texture.h"
#include "shader.h"

namespace engine
{
	class Mesh// : private NonCopyable
    {
    private:
        // render data 
        unsigned int VBO{}, EBO{}, VAO{};

        // mesh Data
        std::vector<Vertex>       vertices{};
        std::vector<unsigned int> indices{};

        std::shared_ptr<Material> m_material{};

    public:
        // constructor
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::shared_ptr<Material>& material);
		~Mesh() = default;

        // render the mesh
        void draw(Shader& shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), float angle = 0.0f, glm::vec3 scale = glm::vec3(1.0f));

        std::shared_ptr<Material> getMaterial() { return m_material; }

        void clean();
       
    private:
        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
