#pragma once

#include <string>
#include <vector>

#include "misc/noncopyable.h"

#include "vertex.h"
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
        std::vector<Texture>      textures{};
    public:
        // constructor
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures);
		~Mesh() = default;

        // render the mesh
        void draw(Shader& shader);
       
    private:
        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
