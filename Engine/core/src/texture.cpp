#include "../include/texture.h"

#include "../include/common_defines.h"

#include "SOIL2.h"

#include <iostream>
#include <functional>

namespace engine {
    namespace TextureManager {
        std::unordered_map<std::string, std::future<std::tuple<unsigned char*, int, int, int>>> textureCache;
        std::queue<std::function<void()>> textureUploadQueue;
        std::mutex textureCacheMutex;
        std::mutex textureQueueMutex;
        std::unordered_map<std::string, unsigned int> textureIDCache;
    }
}

engine::Texture::Texture(unsigned int id, const std::string& type, const std::string& path)
    : id(id), type(type), path(path)
{
}

/// <summary>
/// Synchronous texture loading
/// </summary>
unsigned int engine::Texture::loadTexture(const std::string& filename, bool repeat, bool gammaCorrection)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    //stbi_set_flip_vertically_on_load(true);

    int width{}, height{}, nrComponents{};
    //unsigned char* data = nullptr;// stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

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

        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        SOIL_free_image_data(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}


std::tuple<unsigned char*, int, int, int> engine::Texture::loadTextureAsyncInternal(const std::string& filename)
{
    int width{}, height{}, nrComponents{};
    unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
    
    if (!data) {
        std::cerr << "Texture failed to load at path: " << filename << std::endl;
        return { nullptr, 0, 0, 0 };
    }

    return { data, width, height, nrComponents };
}

/// <summary>
/// Asynchronous texture loading
/// </summary>
unsigned int engine::Texture::loadTextureAsync(const std::string& filename, bool repeat, bool gammaCorrection)
{
    if (filename.empty()) return 0;

    std::lock_guard<std::mutex> lock(engine::TextureManager::textureCacheMutex);

    // Check if the texture is already being loaded asynchronously
    if (engine::TextureManager::textureCache.find(filename) != engine::TextureManager::textureCache.end()) {
        return 0; // Already loading
    }

    // Ensure the future is correctly assigned
    engine::TextureManager::textureCache[filename] = std::async(std::launch::async, [filename]() -> std::tuple<unsigned char*, int, int, int> {
        int width{}, height{}, nrComponents{};
        unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

        if (!data) {
            std::cerr << "Texture failed to load at path: " << filename << std::endl;
            return { nullptr, 0, 0, 0 };
        }
        return { data, width, height, nrComponents };
        });

    return 0;  // Temporary ID, real ID is set later
}

/// <summary>
/// Process Texture Creation on Main Thread
/// </summary>
void engine::Texture::processLoadedTextures()
{
    std::lock_guard<std::mutex> lock(engine::TextureManager::textureQueueMutex);
    while (!engine::TextureManager::textureUploadQueue.empty()) {
        engine::TextureManager::textureUploadQueue.front()(); // Execute OpenGL task
        engine::TextureManager::textureUploadQueue.pop();
    }
}

/// <summary>
/// Enqueue Texture Creation to Run on Main Thread
/// </summary>
unsigned int engine::Texture::enqueueTextureCreation(const std::string& filename, bool generateMipmaps, bool repeat, bool gammaCorrection)
{
    std::lock_guard<std::mutex> lock(engine::TextureManager::textureCacheMutex);

    // 1️⃣ Check if the texture was loaded asynchronously
    auto it = engine::TextureManager::textureCache.find(filename);
    if (it == engine::TextureManager::textureCache.end()) {
        std::cerr << "Warning: Texture future for " << filename << " not found!" << std::endl;
        return 0;  // Exit if the texture is not found in cache
    }

    // 2️⃣ Ensure the future is valid before calling `.get()`
    if (!it->second.valid()) {
        std::cerr << "Warning: Texture future for " << filename << " is invalid!" << std::endl;
        return 0;  // Future is invalid, exit early
    }

    // 3️⃣ Retrieve texture data (blocking call)
    auto [data, width, height, nrComponents] = it->second.get();
    if (!data || width == 0 || height == 0 || nrComponents == 0) {
        std::cerr << "Error: Texture " << filename << " failed to load or is empty!" << std::endl;
        return 0;  // Prevent further processing
    }

    // Queue OpenGL Calls for Execution in `processLoadedTextures()`
    {
        std::lock_guard<std::mutex> lock(engine::TextureManager::textureQueueMutex);

        engine::TextureManager::textureUploadQueue.push([filename, data, width, height, nrComponents, generateMipmaps, repeat, gammaCorrection]()
        {
            unsigned int textureID = createOpenGLTexture(data, width, height, nrComponents, generateMipmaps, repeat, gammaCorrection);

            engine::TextureManager::textureIDCache[filename] = textureID; // Store in cache

            //std::cerr << "EnqueueTextureCreation " << filename << " with TextureID " << textureID << std::endl;

            SOIL_free_image_data(data);  // Free after OpenGL upload

            return textureID;
        });
    }

    return 0;
}

/// <summary>
/// Creates OpenGL Texture (Always Called on Main Thread)
/// </summary>
unsigned int engine::Texture::createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool generateMipmaps, bool repeat, bool gammaCorrection)
{
    if (!data) return 0;

    GLenum format = (nrComponents == 1) ? GL_RED : (nrComponents == 3) ? GL_RGB : GL_RGBA;

    unsigned int textureID{};
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    if (generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    int width = 0, height = 0, nrComponents = 0;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            SOIL_free_image_data(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int engine::Texture::loadHDRImage(const std::string& filename, bool alpha, bool repeat)
{
    unsigned int textureID;
    int width{}, height{}, nrComponents{};

    float* data = SOIL_load_HDR_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
    if (data)
    {
        GLenum internalFormat = (alpha && nrComponents == 4) ? GL_RGBA32F : GL_RGB32F;
        GLenum format = (alpha && nrComponents == 4) ? GL_RGBA : GL_RGB;

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Trilinear filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D); // Improves quality at different distances

        SOIL_free_float_image_data(data);
    }
    else
    {
        std::cout << "HDR texture failed to load at path: " << filename << std::endl;
        SOIL_free_float_image_data(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}