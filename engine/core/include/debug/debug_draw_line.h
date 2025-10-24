#pragma once

#include "../shader.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>


namespace engine
{
    class DebugDraw final
    {
    public:
        DebugDraw();
        ~DebugDraw();
        void init();
        void addLine(
            const glm::vec3& start,
            const glm::vec3& end,
            const glm::vec3& color = glm::vec3(1.0f),
            bool withArrow = false,
            float arrowSize = 0.1f
        );
        void render(const glm::mat4& view, const glm::mat4& projection);
        void clear();
        void clean();

    private:
        struct Line
        {
            glm::vec3 start;
            glm::vec3 end;
            glm::vec3 color;
        };
        std::vector<Line> m_lines;
        GLuint m_vao{ 0 };
        GLuint m_vbo{ 0 };
        Shader m_shader{};
        bool m_initialized{ false };
    };
}
