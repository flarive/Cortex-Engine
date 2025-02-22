#include "../../include/materials/material2.h"

#include <iostream>

void engine::Material2::bind(Shader& shader) const
{
    unsigned int textureUnit = 0;

    for (const auto& texture : textures)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        // Pass the texture unit to the shader
        //shader.setInt("material." + texture.type + "1", textureUnit);

        if (texture.id > 0)
            shader.setBool("material.has_" + texture.type + "_map", true);

        textureUnit++;
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture
}

void engine::Material2::unbind() const
{
    for (size_t i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
}
