#pragma once

#include "primitive.h"

#include "../shader.h"

namespace engine
{
    class Skybox
    {
    public:
        Skybox();

        void setup(std::vector<std::string> faces);

        // draws the model, and thus all its meshes
        void draw(const glm::mat4& projection, const glm::mat4& view);


        // optional: de-allocate all resources once they've outlived their purpose
        void clean();


    private:
        Shader m_skyboxShader;

        unsigned int m_cubemapTexture = 0;

        unsigned int m_skyboxVAO = 0, m_skyboxVBO = 0;
    };
}
