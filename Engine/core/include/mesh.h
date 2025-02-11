#pragma once

#include <string>
#include <vector>

#include "vertex.h"
#include "texture.h"
#include "shader.h"

namespace engine
{
    class Mesh
    {
    public:

        // constructor
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        // render the mesh
        void Draw(Shader& shader);
       
    private:
        // render data 
        unsigned int VBO{}, EBO{};

        // mesh Data
        std::vector<Vertex>       vertices{};
        std::vector<unsigned int> indices{};
        std::vector<Texture>      textures{};
        unsigned int VAO{};


        // initializes all the buffer objects/arrays
        void setupMesh();
    };
}
