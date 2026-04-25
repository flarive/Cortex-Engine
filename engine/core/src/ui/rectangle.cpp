#include "../../include/ui/rectangle.h"

engine::Shader engine::UIRectangle::m_rectShader;

engine::UIRectangle::~UIRectangle()
{
    glDeleteVertexArrays(1, &m_VAO);
}

void engine::UIRectangle::setup(GLFWwindow* window)
{
    m_window = window;

    int width{ 0 }, height{ 0 };
    glfwGetWindowSize(m_window, &width, &height);

    if (!m_rectShader.isInitialized()) {
        m_rectShader.init("UIRectangleShader", "shaders/ui/rectangle.vert", "shaders/ui/rectangle.frag");
    }

    glm::mat4 projection = glm::ortho(0.0f, float(width), 0.0f, float(height));

    m_rectShader.use();
    m_rectShader.setMat4("projection", projection);

    initRenderData();
}

void engine::UIRectangle::draw(glm::vec2 position, glm::vec2 size, float rotate, const Color& fillColor, const Color& borderColor, float borderThickness, float borderRadius)
{
    int width{ 0 }, height{ 0 };
    glfwGetWindowSize(m_window, &width, &height);

    m_rectShader.use();

    // This keeps winding CCW without changing OpenGL state
    glm::mat4 projection = glm::ortho(0.0f, float(width), float(height), 0.0f);
    m_rectShader.setMat4("projection", projection);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0, 0, 1));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    m_rectShader.setMat4("model", model);

    m_rectShader.setVec4("fillColor", { fillColor.r, fillColor.g, fillColor.b, fillColor.a });
    m_rectShader.setVec4("borderColor", { borderColor.r, borderColor.g, borderColor.b, borderColor.a });
    m_rectShader.setVec2("rectSize", size); // rect size in pixels
    m_rectShader.setFloat("radiusPx", borderRadius);    // corner radius in pixels
    m_rectShader.setFloat("borderPx", borderThickness);    // border thickness in pixels


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Send to GPU
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void engine::UIRectangle::initRenderData()
{
    unsigned int VBO{};

    float vertices[] = {
        // pos
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO FIRST
    glBindVertexArray(m_VAO);

    // Then bind VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(float),
        (void*)0
    );

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void engine::UIRectangle::clean()
{
    m_rectShader.clean();
}