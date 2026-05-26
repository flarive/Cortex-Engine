#include "../include/texture.h"

#include "../include/managers/log_manager.h"
#include "../include/common_defines.h"

#include "../include/misc/ltc_matrix.h"

#include "SOIL2.h"

#include <iostream>
#include <functional>




namespace engine {
    namespace TextureManager {
        std::unordered_map<std::string, TextureLoadResult> textureCache;
        std::queue<std::function<void()>> textureUploadQueue;
        std::mutex textureCacheMutex;
        std::mutex textureQueueMutex;
        std::unordered_map<std::string, unsigned int> textureIDCache;
        std::unordered_map<std::string, TextureData> textureDataCache;
    }
}

engine::Texture::Texture(unsigned int id, const std::string& type, const std::string& path)
    : id(id), type(type), path(path)
{
}

void engine::Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

/// <summary>
/// Synchronous texture loading
/// </summary>
unsigned int engine::Texture::loadTexture(const std::string& filename, bool repeat, bool gammaCorrection)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    int width{}, height{}, nrComponents{};
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
        logger.error("Texture failed to load at path: {}", filename);
        SOIL_free_image_data(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}

/// <summary>
/// Synchronous texture loading
/// </summary>
engine::TextureData engine::Texture::loadTextureExtended(const std::string& filename, bool repeat, bool gammaCorrection)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    int width{}, height{}, nrComponents{};
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
    {
        logger.error("Texture failed to load at path: {}", filename);
        SOIL_free_image_data(data);
        exit(EXIT_FAILURE);
    }

    return { textureID, data, width, height, nrComponents };
}

/// <summary>
/// Asynchronous texture loading
/// </summary>
unsigned int engine::Texture::loadTextureAsync(const std::string& filename, bool repeat, bool invertY, bool gammaCorrection)
{
    if (filename.empty()) return 0;

    std::lock_guard<std::mutex> lock(engine::TextureManager::textureCacheMutex);

    // Check if the texture is already being loaded asynchronously
    if (engine::TextureManager::textureCache.find(filename) != engine::TextureManager::textureCache.end()) {
        return 0; // Already loading
    }

    logger.info("Loading async texture {}", filename);

    // Ensure the future is correctly assigned
    engine::TextureManager::textureCache[filename] = {
    std::async(std::launch::async, [filename, invertY]() -> std::tuple<unsigned char*, int, int, int> {
        int width{}, height{}, nrComponents{};
        unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);
        if (!data) {
            logger.error("Texture failed to load at path: {}", filename);
            return { nullptr, 0, 0, 0 };
        }

        // Flip the image vertically
        if (invertY)
        {
            unsigned char* flippedData = flipImageVertically(data, width, height, nrComponents);
            return { flippedData, width, height, nrComponents };
        }

        return { data, width, height, nrComponents };
    }),
    false,
    {} // Result is empty initially
    };

    return 0;  // Temporary ID, real ID is set later
}

unsigned char* engine::Texture::flipImageVertically(unsigned char* data, int width, int height, int nrComponents)
{
    unsigned char* flippedData = new unsigned char[width * height * nrComponents];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < nrComponents; c++) {
                flippedData[(height - 1 - y) * width * nrComponents + x * nrComponents + c] =
                    data[y * width * nrComponents + x * nrComponents + c];
            }
        }
    }
    delete[] data;
    return flippedData;
}

/// <summary>
/// Process Texture Creation on Main Thread
/// </summary>
void engine::Texture::processLoadedTextures()
{
    std::lock_guard<std::mutex> lock(engine::TextureManager::textureQueueMutex);

    while (!engine::TextureManager::textureUploadQueue.empty())
    {
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

    //std::cerr << "EnqueueTextureCreation " << filename << std::endl;

    // 1️. Check if the texture was loaded asynchronously
    auto it = engine::TextureManager::textureCache.find(filename);
    if (it == engine::TextureManager::textureCache.end())
    {
        logger.warn("Texture future for {} not found !", filename);
        return 0;  // Exit if the texture is not found in cache
    }

    // 2️. Ensure the future is valid before calling `.get()`
    auto& entry = engine::TextureManager::textureCache[filename];
    if (!entry.ready) {
        if (!entry.future.valid()) {
            logger.warn("Texture future for {} is invalid !", filename);
            return 0;
        }

        entry.result = entry.future.get();  // Retrieve once
        entry.ready = true;  // Mark it as ready
    }

    // 3️. Retrieve texture data (blocking call)
    //auto [data, width, height, nrComponents] = it->second.get();
    auto [data, width, height, nrComponents] = entry.result;
    if (!data || width == 0 || height == 0 || nrComponents == 0) {
        logger.error("Texture {} failed to load or is empty !", filename);
        return 0;  // Prevent further processing
    }

    // Avoid duplicate OpenGL uploads
    if (engine::TextureManager::textureIDCache.find(filename) != engine::TextureManager::textureIDCache.end())
    {
        return engine::TextureManager::textureIDCache[filename];  // Already created
    }

    // Queue OpenGL Calls for Execution in `processLoadedTextures()`
    {
        std::lock_guard<std::mutex> lock(engine::TextureManager::textureQueueMutex);

        engine::TextureManager::textureUploadQueue.push([filename, data, width, height, nrComponents, generateMipmaps, repeat, gammaCorrection]()
        {
            //  OpenGL upload texture
            unsigned int textureID = createOpenGLTexture(data, width, height, nrComponents, generateMipmaps, repeat, gammaCorrection);

            engine::TextureManager::textureIDCache[filename] = textureID; // Store in cache
            engine::TextureManager::textureDataCache[filename] = TextureData{ textureID, nullptr, width, height, nrComponents }; // Cache for later use

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

    // Flip image vertically
    int rowSize = width * nrComponents;  // Number of bytes per row
    unsigned char* rowBuffer = new unsigned char[rowSize];

    for (int y = 0; y < height / 2; ++y) {
        unsigned char* rowTop = data + y * rowSize;
        unsigned char* rowBottom = data + (height - y - 1) * rowSize;

        std::memcpy(rowBuffer, rowTop, rowSize);
        std::memcpy(rowTop, rowBottom, rowSize);
        std::memcpy(rowBottom, rowBuffer, rowSize);
    }

    delete[] rowBuffer;

    // Create and bind OpenGL texture
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
            logger.error("Cubemap texture failed to load at path: {}", faces[i]);
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
        logger.error("HDR texture failed to load at path: {}", filename);
        SOIL_free_float_image_data(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}




unsigned int engine::Texture::loadTextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    logger.info("Loading texture {}", filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    //int width, height, nrComponents;
    int width = 0, height = 0, nrComponents = 0;
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        SOIL_free_image_data(data);
        exit(EXIT_FAILURE);
    }

    return textureID;
}

unsigned int engine::Texture::loadGLTextureFromFile(const char* path, const std::string& directory, bool invertY, bool mipmaps, bool compress)
{
    std::string filename{};

    if (directory.empty())
        filename = path;
    else
        filename = directory + '/' + path;

    logger.info("Loading openGL texture {}", filename);

    unsigned int flags = 0;

    if (invertY)
		flags |= SOIL_FLAG_INVERT_Y;

    if (mipmaps)
        flags |= SOIL_FLAG_MIPMAPS;

    if (compress)
        flags |= SOIL_FLAG_COMPRESS_TO_DXT;

    unsigned int textureID = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);

    if (textureID == 0)
    {
        std::cout << "Texture failed to load at path: " << path << "\n";
        std::cout << "SOIL error: " << SOIL_last_result() << "\n";
        //throw std::runtime_error("Texture failed to load");
    }

    // Set texture parameters manually, since SOIL does not handle all of them
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}


unsigned int engine::Texture::loadTextureFromMemory(const unsigned char* data, size_t size, const char* filename)
{
    int width = 0, height = 0, channels = 0;

    logger.info("Loading memory texture {}", filename);

    // Load image from memory buffer using SOIL
    unsigned char* image = SOIL_load_image_from_memory(data, static_cast<int>(size), &width, &height, &channels, SOIL_LOAD_AUTO);

    if (!image)
    {
        std::cerr << "Failed to load embedded texture from memory." << std::endl;
        return 0;
    }

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(image);

    return textureID;
}

unsigned int engine::Texture::loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height)
{
    if (!data || height == 0 || width == 0)
    {
        std::cerr << "Invalid uncompressed texture." << std::endl;
        return 0;
    }

    unsigned int textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Each pixel is an aiTexel (RGBA8888)
    //const unsigned char* pixelData = reinterpret_cast<const unsigned char*>(texture->pcData);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

GLuint engine::Texture::loadMTexture()
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

GLuint engine::Texture::loadLUTTexture()
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_FLOAT, LTC2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void engine::Texture::checkTextureIsValid(unsigned int textureID)
{
    // After loading the textures, check if they are valid
    GLint width2, height2, internalFormat;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width2);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height2);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    std::cout << "textureID: " << textureID << ", width: " << width2 << ", height: " << height2 << ", format: " << internalFormat << std::endl;
}

engine::TextureData engine::Texture::getTextureData(const std::string& texturePath)
{
    if (engine::TextureManager::textureDataCache.find(texturePath) != engine::TextureManager::textureDataCache.end()) {
        return engine::TextureManager::textureDataCache[texturePath];
    }

	return {}; // Return default if not found
}

