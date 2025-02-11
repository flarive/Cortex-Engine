#include "../../include/primitives/primitive.h"

void engine::Primitive::clean()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);

    m_VAO = 0;
    m_VBO = 0;
    m_EBO = 0;
}