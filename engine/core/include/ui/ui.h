#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../misc/colors.h"

#include "../shader.h"

#include <glm/glm.hpp>

namespace engine
{
    class UIBase : private NonCopyable
    {
    public:
        UIBase();
        virtual ~UIBase();

        bool isEnabled() const { return m_isEnabled; }
        void setEnabled(bool enabled) { m_isEnabled = enabled; }

        virtual void clean() = 0;

    protected:
        bool m_isEnabled{ true };
        GLFWwindow* m_window{};
        unsigned int m_VAO{}, m_VBO{};
    };
}