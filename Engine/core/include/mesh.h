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
	class Mesh : private NonCopyableButMovable
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
        void draw(Shader& shader, glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), float angle = 0.0f, glm::vec3 scale = glm::vec3(1.0f));

        void draw(Shader& shader, const glm::mat4 model);

        std::shared_ptr<Material> getMaterial() { return m_material; }

        void clean();
       
    private:
        // render data 
        unsigned int VBO{}, EBO{}, VAO{};

        std::shared_ptr<Material> m_material{};

        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
