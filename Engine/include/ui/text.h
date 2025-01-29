#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../common_defines.h"

#include <map>

#include "../character.h"
#include "../shader.h"
#include "../file_system.h"

namespace engine
{
    class Text
    {
    public:
        Text();


        void setup();


        // draws the model, and thus all its meshes
        // render line of text
        // -------------------
        void draw(std::string text, float x, float y, float scale, glm::vec3 color);
        

    private:

        // render data 
        std::map<GLchar, Character> m_characters;
        unsigned int m_VAO, m_VBO;

        Shader m_textShader;

        unsigned int SCR_WIDTH = 800;
        unsigned int SCR_HEIGHT = 600;
    };
}
