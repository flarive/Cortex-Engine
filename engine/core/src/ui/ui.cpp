#include "../../include/ui/ui.h"

engine::UIBase::~UIBase()
{
    
}


void engine::UIBase::clean()
{
    // Delete VAO and VBO
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

}