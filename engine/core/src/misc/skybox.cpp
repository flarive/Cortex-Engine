#include "../../include/misc/skybox.h"

#include "../../include/managers/texture_manager.h"
#include "../../include/primitives/primitive.h"

#include "../../include/debug/opengl_debug.h"

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
    m_cubemapTexture = engine::TextureManager::loadCubemap(faces);

    m_isSetup = true;
}

// draws the model, and thus all its meshes
void engine::Skybox::draw(const glm::mat4& projection, const glm::mat4& view)
{
    if (!m_skyboxShader.isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (m_cubemapTexture <= 0) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (m_skyboxVAO == 0 || m_skyboxVBO == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }
    
    // draw skybox as last
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
    glDepthMask(GL_FALSE);

    m_skyboxShader.use();
    OpenGLDebug::checkGLError("shader.use77");
    m_skyboxShader.setMat4("view", glm::mat4(glm::mat3(view))); // remove translation from the view matrix
    m_skyboxShader.setMat4("projection", projection);
    m_skyboxShader.setInt("texture_skybox", 0);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
    

    // Send skybox cube to GPU
    glBindVertexArray(m_skyboxVAO);
    OpenGLDebug::checkGLError("glBindVertexArray");

    glDrawArrays(GL_TRIANGLES, 0, 36);
    OpenGLDebug::checkGLError("glDrawArrays");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    

	// unbind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //glUseProgram(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS); // set depth function back to default
}

// optional: de-allocate all resources once they've outlived their purpose
void engine::Skybox::clean()
{
    if (this && m_isSetup)
    {
        glDeleteVertexArrays(1, &m_skyboxVAO);
        glDeleteBuffers(1, &m_skyboxVBO);

        glDeleteTextures(1, &m_cubemapTexture);

        m_skyboxShader.clean();

        m_isSetup = false;
    }
}