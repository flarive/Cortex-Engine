#include "../include/texture.h"

#include "../include/common_defines.h"

#include "../include/tools/file_system.h"


#define STBI_FAILURE_USERMSG //generate user friendly error messages
#define STB_IMAGE_IMPLEMENTATION // for HDR support
#include "stb_image.h"



engine::Texture::Texture(unsigned int id, const std::string& type, const std::string& path)
    : id(id), type(type), path(path)
{

}

unsigned int engine::Texture::loadTexture(const std::string& filename, bool repeat, bool gammaCorrection)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);

    int width{}, height{}, nrComponents{};
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format{};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}

unsigned int engine::Texture::createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    unsigned int texture{};
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


unsigned int engine::Texture::loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    stbi_set_flip_vertically_on_load(false);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

//unsigned int engine::Texture::loadHDRImage(const std::string& filename, bool alpha, bool repeat)
//{
//    int width{}, height{}, nrComponents{};
//
//    stbi_set_flip_vertically_on_load(true);
//
//    float* data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
//    unsigned int hdrTexture{};
//    if (data)
//    {
//        glGenTextures(1, &hdrTexture);
//        glBindTexture(GL_TEXTURE_2D, hdrTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA16F : GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cerr << "Failed to load HDR image." << stbi_failure_reason() << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    return hdrTexture;
//}

unsigned int engine::Texture::loadHDRImage(const std::string& filename, bool alpha, bool repeat)
{
    int width{}, height{}, nrComponents{};
    stbi_set_flip_vertically_on_load(true);

    float* data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
    if (!data)
    {
        std::cerr << "Failed to load HDR image: " << filename << " - " << stbi_failure_reason() << std::endl;
        return 0; // Return invalid texture ID instead of exiting.
    }

    GLenum internalFormat = (alpha && nrComponents == 4) ? GL_RGBA32F : GL_RGB32F;
    GLenum format = (alpha && nrComponents == 4) ? GL_RGBA : GL_RGB;

    unsigned int hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Trilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D); // Improves quality at different distances

    stbi_image_free(data);

    return hdrTexture;
}

