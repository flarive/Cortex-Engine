#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../debug/opengl_debug.h"

#include <map>

#include "../character.h"

#include "ui.h"

namespace engine
{
    class UIText final : public UIBase
    {
    public:
        UIText() = default;
        ~UIText() = default;

        void setup(GLFWwindow* window, const std::string& fontPath, int fontSize);

        // draws the model, and thus all its meshes
        // render line of text
        // -------------------
        void draw(const std::string& text, float x, float y, float scale, const Color& color = Colors::White);

        glm::vec2 measure(const std::string& text, float scale = 1.0f) const;

        void clean() override;

    private:
        std::map<GLchar, Character> m_characters{};
        static Shader m_textShader; // Shared across all instances
    };
}
