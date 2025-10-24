#include "../../include/debug/debug_draw_line.h"

#include <glad/glad.h>
#include <iostream>

engine::DebugDraw::DebugDraw() {}
engine::DebugDraw::~DebugDraw()
{
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
}

void engine::DebugDraw::init()
{
    if (m_initialized) return;
    m_shader.init("debug_line", "shaders/debug/debug_line.vert", "shaders/debug/debug_line.frag");
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBindVertexArray(0);
    m_initialized = true;
}

void engine::DebugDraw::addLine(
    const glm::vec3& start,
    const glm::vec3& end,
    const glm::vec3& color,
    bool withArrow,
    float arrowSize
) {
    // Add the main line
    m_lines.push_back({ start, end, color });

    if (withArrow) {
        // Calculate the direction vector
        glm::vec3 direction = glm::normalize(end - start);
        // Calculate the perpendicular vectors for the arrowhead
        glm::vec3 perpendicular1 = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 perpendicular2 = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f)));

        // If the cross product is zero, use a default perpendicular vector
        if (glm::length(perpendicular1) < 0.01f) {
            perpendicular1 = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        if (glm::length(perpendicular2) < 0.01f) {
            perpendicular2 = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // Calculate the arrowhead points
        glm::vec3 arrowPoint1 = end - direction * arrowSize + perpendicular1 * arrowSize;
        glm::vec3 arrowPoint2 = end - direction * arrowSize + perpendicular2 * arrowSize;

        glm::vec3 arrowPoint3 = end - direction * arrowSize - perpendicular1 * arrowSize;
        glm::vec3 arrowPoint4 = end - direction * arrowSize - perpendicular2 * arrowSize;

        // Add the arrowhead lines
        m_lines.push_back({ end, arrowPoint1, color });
        m_lines.push_back({ end, arrowPoint2, color });

        m_lines.push_back({ end, arrowPoint3, color });
        m_lines.push_back({ end, arrowPoint4, color });
    }
}
//
//void engine::DebugDraw::addLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, bool withArrow, float arrowSize)
//{
//    // Add the main line
//    m_lines.push_back({ start, end, color });
//
//    if (withArrow) {
//        // Calculate the direction vector
//        glm::vec3 direction = glm::normalize(end - start);
//
//        // Find a perpendicular vector to the direction
//        glm::vec3 perpendicular;
//        if (fabs(direction.x) > fabs(direction.z)) {
//            // If direction is mostly in the x-axis, cross with the y-axis
//            perpendicular = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
//        }
//        else {
//            // Otherwise, cross with the x-axis
//            perpendicular = glm::normalize(glm::cross(direction, glm::vec3(1.0f, 0.0f, 0.0f)));
//        }
//
//        // Calculate the two symmetrical points for the arrowhead
//        glm::vec3 arrowPoint1 = end - direction * arrowSize + perpendicular * arrowSize;
//        glm::vec3 arrowPoint2 = end - direction * arrowSize - perpendicular * arrowSize;
//
//        // Add the two lines for the symmetrical arrowhead
//        m_lines.push_back({ end, arrowPoint1, color });
//        m_lines.push_back({ end, arrowPoint2, color });
//    }
//}

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

void engine::DebugDraw::clean()
{
    m_shader.clean();
    m_initialized = false;
    m_lines.clear();

    // delete VAO/VBO
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}