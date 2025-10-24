#include "../../include/misc/skybox.h"

#include "../../include/texture.h"
#include "../../include/primitives/primitive.h"

void engine::Skybox::setup(const std::vector<std::string>& faces)
{
    m_skyboxShader.init("skybox", "shaders/skybox.vert", "shaders/skybox.frag");


    // skybox VAO
    glGenVertexArrays(1, &m_skyboxVAO);
    glGenBuffers(1, &m_skyboxVBO);
    glBindVertexArray(m_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);



    // skybox textures
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    m_cubemapTexture = engine::Texture::loadCubemap(faces);


    m_skyboxShader.use();
    m_skyboxShader.setInt("texture_skybox", 0);
}

// draws the model, and thus all its meshes
void engine::Skybox::draw(const glm::mat4& projection, const glm::mat4& view)
{
    // draw skybox as last
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
    glDepthMask(GL_FALSE);
   
    m_skyboxShader.use();
    m_skyboxShader.setMat4("view", glm::mat4(glm::mat3(view))); // remove translation from the view matrix
    m_skyboxShader.setMat4("projection", projection);

    // Send skybox cube to GPU
    glBindVertexArray(m_skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS); // set depth function back to default
}

// optional: de-allocate all resources once they've outlived their purpose
void engine::Skybox::clean()
{
    // CRASH !!!!!!!!!!!!
    glDeleteVertexArrays(1, &m_skyboxVAO);
    glDeleteBuffers(1, &m_skyboxVBO);
}