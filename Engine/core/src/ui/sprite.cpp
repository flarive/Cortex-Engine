#include "../../include/ui/sprite.h"

#include "../../include/texture.h"

#include <tuple>

engine::Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &m_quadVAO);
}

void engine::Sprite::setup(GLFWwindow* window, const std::string& filepath)
{
    m_window = window;

    int width{ 0 };
    int height{ 0 };
    glfwGetWindowSize(m_window, &width, &height);
    
    m_spriteShader.init("UISpriteShader", "shaders/sprite.vertex", "shaders/sprite.frag");

    glm::mat4 projection2 = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    m_spriteShader.use();
    m_spriteShader.setMat4("projection", projection2);

    auto textureTuple = Texture::loadTextureExtended(filepath, false, false);
    
    m_texture_id = std::get<0>(textureTuple);
    width = std::get<2>(textureTuple);
    height = std::get<3>(textureTuple);

    initRenderData();
}

void engine::Sprite::draw(glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color)
{
    int width{ 0 };
    int height{ 0 };
    glfwGetWindowSize(m_window, &width, &height);
    
    // prepare transformations
    m_spriteShader.use();

    glm::mat4 projection2 = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    m_spriteShader.setMat4("projection", projection2);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

    m_spriteShader.setMat4("model", model);

    // render textured quad
    m_spriteShader.setVec3("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);


    if (size.x != 1.0f && size.y != 1.0f)
    {
        // set sprite size to scaled display size
        width = static_cast<int>(size.x);
        height = static_cast<int>(size.y);
    }
}

void engine::Sprite::initRenderData()
{
    // configure VAO/VBO
    unsigned int VBO{0};
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}