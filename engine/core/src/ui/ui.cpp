#include "../../include/ui/ui.h"

#include "../../include/managers/log_manager.h"

engine::UIBase::UIBase()
{
    logger.trace("UI base constructor called");
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

engine::UIBase::~UIBase()
{
    logger.trace("UI base destructor called");
}