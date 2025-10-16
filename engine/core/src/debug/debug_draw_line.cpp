#include "../../include/debug/debug_draw_line.h"

#include <glad/glad.h>
#include <iostream>

engine::DebugDraw::DebugDraw()
{
}

engine::DebugDraw::~DebugDraw()
{
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
}

void engine::DebugDraw::init()
{
    if (m_initialized) return;

    // Create shader
    m_shader.init("debug_line", "shaders/debug/debug_line.vert", "shaders/debug/debug_line.frag");

    // Create VAO/VBO
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);

    m_initialized = true;
}

void engine::DebugDraw::addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
    m_lines.push_back({ start, end, color });
}

void engine::DebugDraw::render(const glm::mat4& view, const glm::mat4& projection)
{
    if (!m_initialized || m_lines.empty()) return;

    m_shader.use();
    m_shader.setMat4("view", view);
    m_shader.setMat4("projection", projection);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    for (const auto& line : m_lines)
    {
        glm::vec3 vertices[2] = { line.start, line.end };
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        m_shader.setVec3("color", line.color);
        glDrawArrays(GL_LINES, 0, 2);
    }

    glBindVertexArray(0);
}

void engine::DebugDraw::clear()
{
    m_lines.clear();
}

