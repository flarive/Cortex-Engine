#include "../include/texture.h"

#include "../include/common_defines.h"

#include "../include/file_system.h"

#include "SOIL2.h"

unsigned int engine::Texture::loadImage(std::string filename, bool alpha, bool repeat, bool gammaCorrection)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (repeat)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // load and generate the texture
    int width, height, nrComponents;
    unsigned char* data = SOIL_load_image(file_system::getPath(filename).c_str(), &width, &height, &nrComponents, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    if (data)
    {
        GLenum internalFormat{};
        GLenum dataFormat{};
        if (nrComponents == 1)
        {
            internalFormat = dataFormat = GL_RED;
        }
        else if (nrComponents == 3)
        {
            internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }
        
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load image" << std::endl;
        exit(EXIT_FAILURE);
    }

    SOIL_free_image_data(data);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind

    return texture;
}


unsigned int engine::Texture::loadTexture(std::string filename, bool repeat, bool gammaCorrection)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    texture = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
    (
        file_system::getPath(filename).c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
    );

    if (texture == 0)
    {
        std::cerr << "Failed to load texture" << std::endl;
        exit(EXIT_FAILURE);
    }

    glBindTexture(GL_TEXTURE_2D, texture);

    // Apply repeat or clamp wrapping mode
    GLint wrapMode = repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Apply gamma correction if enabled
    if (gammaCorrection)
    {
        // Retrieve texture parameters to determine format
        int width, height, format;
        glBindTexture(GL_TEXTURE_2D, texture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);

        // Determine sRGB equivalent
        GLenum srgbFormat = (format == GL_RGBA || format == GL_RGBA8) ? GL_SRGB_ALPHA : GL_SRGB;

        // Re-upload texture data with sRGB internal format
        unsigned char* imageData = SOIL_load_image(file_system::getPath(filename).c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
        if (imageData) {
            glTexImage2D(GL_TEXTURE_2D, 0, srgbFormat, width, height, 0, (format == GL_RGBA ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, imageData);
            glGenerateMipmap(GL_TEXTURE_2D);
            SOIL_free_image_data(imageData);
        }
        else {
            std::cerr << "Failed to reload image for gamma correction." << std::endl;
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind

    return texture;
}

unsigned int engine::Texture::createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Define a single pixel with the given color
    unsigned char color[] = { r, g, b, a };

    // Upload the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set wrapping mode (clamp to edge since it’s a single pixel)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind

    return texture;
}


unsigned int engine::Texture::loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        bool alpha = false;
        unsigned char* data = SOIL_load_image(file_system::getPath(faces[i]).c_str(), &width, &height, &nrChannels, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            SOIL_free_image_data(data);
        }
        else
        {
            std::cerr << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            SOIL_free_image_data(data);
            exit(EXIT_FAILURE);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}