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
        unsigned int VBO{}, EBO{};

        // mesh Data
        std::vector<Vertex>       vertices{};
        std::vector<unsigned int> indices{};
        std::vector<Texture>      textures{};

        unsigned int VAO{};

    public:
        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        // render the mesh
        void Draw(Shader& shader);
       
    private:
        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
