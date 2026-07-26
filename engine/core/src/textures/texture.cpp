#include "../../include/textures/texture.h"

#include "../../include/managers/log_manager.h"
#include "../../include/common_defines.h"

#include "../../include/misc/ltc_matrix.h"

#include "../../include/textures/ktx_loader.h"


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

//engine::Texture::Texture()
//{
//    //logger.trace("Texture constructor called");
//}

engine::Texture::Texture(unsigned int id, const std::string& type, const std::string& path)
    : id(id), type(type), path(path)
{
    //logger.trace("Texture constructor called");
}

void engine::Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

/// <summary>
/// Synchronous texture loading
/// </summary>
unsigned int engine::Texture::loadTexture(const std::string& filename, TextureFlags flags)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    // Detect normal maps by filename
    bool isNormalMap = Texture::isNormalMap(filename);

    // Detect heightmaps
	bool isHeightMap = Texture::isHeightMap(filename);


    int width{}, height{}, nrComponents{};
    unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

    if (hasFlag(flags, TextureFlag_InvertY))
        flipImageVertically2(data, width, height, nrComponents);

    if (data)
    {
        GLenum externalFormat{};
        if (nrComponents == 1) externalFormat = GL_RED;
        else if (nrComponents == 3) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB : GL_RGB;
        else if (nrComponents == 4) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB_ALPHA : GL_RGBA;

        // Choose compressed internal format
        GLenum internalFormat{};

        if (isCompressedFile(filename))
        {
            // use GPU texture compression in VRAM
            if (isNormalMap)
                internalFormat = GL_COMPRESSED_RG_RGTC2; // normal maps use BC5 (RGTC2)
            else if (isHeightMap)
                internalFormat = GL_COMPRESSED_RED_RGTC1; // height map use BC4 (RGTC1)
            else
                internalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM : GL_COMPRESSED_RGBA_BPTC_UNORM; // Color textures use BC7
        }

        // Upload texture to GPU with or without compression
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, isCompressedFile(filename) ? internalFormat : externalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);
        // glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // mipmaps
        if (hasFlag(flags, TextureFlag_GenerateMipmaps))
            glGenerateMipmap(GL_TEXTURE_2D);

        // wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

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
/// Synchronous texture loading with extended result
/// </summary>
engine::TextureData engine::Texture::loadTextureExtended(const std::string& filename, TextureFlags flags)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    int width{}, height{}, nrComponents{};
    unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

    if (hasFlag(flags, TextureFlag_InvertY))
        flipImageVertically2(data, width, height, nrComponents);

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

        if (hasFlag(flags, TextureFlag_GenerateMipmaps))
            glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

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
unsigned int engine::Texture::requestLoadTextureAsync(const std::string& filename)
{
    if (filename.empty()) return 0;

    std::lock_guard<std::mutex> lock(engine::TextureManager::textureCacheMutex);

    // Check if the texture is already being loaded asynchronously
    if (engine::TextureManager::textureCache.find(filename) != engine::TextureManager::textureCache.end()) {
        return 0; // Already loading
    }

    logger.info("Loading async texture {}", filename);

    engine::TextureManager::textureCache[filename] =
    {
        std::async(std::launch::async, [filename]() -> TexturePayload
        {
            TexturePayload payload;

            if (isKTX2File(filename))
            {
				// compressed texture, use ktx loader
                // ktx lib can load ktx and ktx2
                payload.type = TextureSourceType::KTXTexture;

                payload.ktxData = ktxLoader::loadKTX(
                    filename,
                    isNormalMap(filename),
                    isHeightMap(filename)
                );

                if (!payload.ktxData)
                    return payload;

                payload.width = payload.ktxData->baseWidth;
                payload.height = payload.ktxData->baseHeight;
                payload.components = ktxLoader::getKTXComponents(payload.ktxData, isNormalMap(filename), isHeightMap(filename));

                return payload;
            }
            else
            {
				// uncompressed texture, use SOIL loader
                // SOIL can load jpg, png, dds...
                payload.type = TextureSourceType::RawPixels;

                payload.rawData = SOIL_load_image(
                    filename.c_str(),
                    &payload.width,
                    &payload.height,
                    &payload.components,
                    SOIL_LOAD_AUTO
                );

                return payload;
            }
        }),
        false,
        {}
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

void engine::Texture::flipImageVertically2(unsigned char* data, int width, int height, int nrComponents)
{
    if (!data) return;

    const int rowSize = width * nrComponents;
    std::vector<unsigned char> rowBuffer(rowSize);

    for (int y = 0; y < height / 2; ++y)
    {
        unsigned char* rowTop = data + y * rowSize;
        unsigned char* rowBottom = data + (height - 1 - y) * rowSize;

        std::memcpy(rowBuffer.data(), rowTop, rowSize);
        std::memcpy(rowTop, rowBottom, rowSize);
        std::memcpy(rowBottom, rowBuffer.data(), rowSize);
    }
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
unsigned int engine::Texture::enqueueAsyncTextureCreation(const std::string& filename, TextureFlags flags)
{
    std::lock_guard<std::mutex> lock(engine::TextureManager::textureCacheMutex);

    bool isNormalMap = Texture::isNormalMap(filename);
    bool isHeightMap = Texture::isHeightMap(filename);
    bool isCompressed = isCompressedFile(filename);

    // 1. Check async load result exists
    auto it = engine::TextureManager::textureCache.find(filename);
    if (it == engine::TextureManager::textureCache.end())
    {
        logger.warn("Texture future for {} not found !", filename);
        return 0;
    }

    auto& entry = it->second;

    // 2. Retrieve future result once
    if (!entry.ready)
    {
        if (!entry.future.valid())
        {
            logger.warn("Texture future for {} is invalid !", filename);
            return 0;
        }

        entry.result = entry.future.get();
        entry.ready = true;
    }

    const TexturePayload& payload = entry.result;

    // 3. Validate payload
    if (payload.type == TextureSourceType::RawPixels)
    {
        if (!payload.rawData || payload.width == 0 || payload.height == 0)
        {
            logger.error("Raw texture {} failed to load !", filename);
            return 0;
        }
    }
    else if (payload.type == TextureSourceType::KTXTexture)
    {
        if (!payload.ktxData)
        {
            logger.error("KTX texture {} failed to load !", filename);
            return 0;
        }
    }

    // 4. Avoid duplicate uploads
    if (engine::TextureManager::textureIDCache.contains(filename))
        return engine::TextureManager::textureIDCache[filename];

    // 5. Enqueue OpenGL upload on main thread
    {
        std::lock_guard<std::mutex> lock(engine::TextureManager::textureQueueMutex);

        engine::TextureManager::textureUploadQueue.push(
            [filename, payload, isCompressed, isNormalMap, isHeightMap, flags]()
            {
                GLuint textureID = 0;

                if (payload.type == TextureSourceType::RawPixels)
                {
                    textureID = createOpenGLTexture(
                        payload.rawData,
                        payload.width,
                        payload.height,
                        payload.components,
                        isCompressed,
                        isNormalMap,
                        isHeightMap,
                        flags
                    );

                    SOIL_free_image_data(payload.rawData);
                }
                else if (payload.type == TextureSourceType::KTXTexture)
                {
                    textureID = ktxLoader::uploadKTX_OpenGL(payload.ktxData);

                    ktxTexture_Destroy(payload.ktxData);
                }

                // Cache result
                engine::TextureManager::textureIDCache[filename] = textureID;
                engine::TextureManager::textureDataCache[filename] = TextureData{ textureID, nullptr, payload.width, payload.height, payload.components };

                return textureID;
            }
        );
    }

    return 0;
}

/// <summary>
/// Creates OpenGL Texture and upload it on the GPU (Always Called on Main Thread)
/// </summary>
unsigned int engine::Texture::createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool isCompressed, bool isNormalMap, bool isHeightMap, TextureFlags flags)
{
    if (!data) return 0;

    // Flip vertically if requested
    if (hasFlag(flags, TextureFlag_InvertY))
        flipImageVertically2(data, width, height, nrComponents);

    // Create and bind OpenGL texture
    unsigned int textureID{};
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    GLenum externalFormat{};
    if (nrComponents == 1) externalFormat = GL_RED;
    else if (nrComponents == 3) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB : GL_RGB;
    else if (nrComponents == 4) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB_ALPHA : GL_RGBA;

    // Choose compressed internal format
    GLenum internalFormat{};
    if (isCompressed)
        internalFormat = chooseCompressedFormat(isNormalMap, isHeightMap, hasFlag(flags, TextureFlag_GammaCorrect), gpuSupportsBC7());



    // Upload texture to GPU with or without compression
    glTexImage2D(GL_TEXTURE_2D, 0, isCompressed ? internalFormat : externalFormat, width, height, 0,  externalFormat, GL_UNSIGNED_BYTE, data);

    
    // ensure texture was compressed
    if (isCompressed)
    {
        GLint compressed;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
        if (!compressed) {
            logger.warn("Driver did not compress texture, falling back to uncompressed.");
        }
    }


    // mipmaps
    if (hasFlag(flags, TextureFlag_GenerateMipmaps))
        glGenerateMipmap(GL_TEXTURE_2D);

    // wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

    return textureID;
}

/// <summary>
/// Uncompressed texture (PNG/JPG/TGA/HDR)
/// Small texture size on disk but using more VRAM on GPU
/// </summary>
/// <param name="filename"></param>
/// <param name="flags"></param>
/// <returns></returns>
unsigned int engine::Texture::createUncompressedTexture(const std::string& filename, TextureFlags flags)
{
    return 0;
}

/// <summary>
/// Compressed texture (DDS/KTX2 (BC7 / BC5 / BC4))
/// Bigger size on disk but using less VRAM on GPU
/// </summary>
/// <param name="filename"></param>
/// <param name="flags"></param>
/// <returns></returns>
unsigned int engine::Texture::createCompressedTexture(const std::string& filename, TextureFlags flags)
{
    return 0;
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

unsigned int engine::Texture::loadHDRImage(const std::string& filename, bool alpha, TextureFlags flags)
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

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

unsigned int engine::Texture::loadTextureFromFile(const char* path, const std::string& directory, TextureFlags flags)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    logger.info("Loading texture {}", filename);

    unsigned int textureID;
    glGenTextures(1, &textureID);

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

        if (hasFlag(flags, TextureFlag_GenerateMipmaps))
            glGenerateMipmap(GL_TEXTURE_2D);

        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

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

unsigned int engine::Texture::loadGLTextureFromFile(const char* path, const std::string& directory, TextureFlags flags)
{
    std::string filename{};

    if (directory.empty())
        filename = path;
    else
        filename = directory + '/' + path;

    logger.info("Loading openGL texture {}", filename);

    unsigned int soilFlags = 0;

    if (hasFlag(flags, TextureFlag_InvertY))
        soilFlags |= SOIL_FLAG_INVERT_Y;

    if (hasFlag(flags, TextureFlag_GenerateMipmaps))
        soilFlags |= SOIL_FLAG_MIPMAPS;

    /*if (hasFlag(flags, TextureFlag_CompressTexture))
        soilFlags |= SOIL_FLAG_COMPRESS_TO_DXT;*/

    unsigned int textureID = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, soilFlags);

    if (textureID == 0)
    {
        std::cout << "Texture failed to load at path: " << path << "\n";
        std::cout << "SOIL error: " << SOIL_last_result() << "\n";
        //throw std::runtime_error("Texture failed to load");
    }

    // Set texture parameters manually, since SOIL does not handle all of them
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

    return textureID;
}

unsigned int engine::Texture::loadTextureFromMemory(const unsigned char* data, size_t size, const char* filename, TextureFlags flags)
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

    if (hasFlag(flags, TextureFlag_GenerateMipmaps))
        glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

    SOIL_free_image_data(image);

    return textureID;
}

unsigned int engine::Texture::loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height, TextureFlags flags)
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if (hasFlag(flags, TextureFlag_GenerateMipmaps))
        glGenerateMipmap(GL_TEXTURE_2D);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

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

bool engine::Texture::isNormalMap(const std::string& filename)
{
    return filename.find("normal") != std::string::npos || filename.find("_n") != std::string::npos || filename.find("_norm") != std::string::npos;
}

bool engine::Texture::isHeightMap(const std::string& filename)
{
    return filename.find("height") != std::string::npos || filename.find("_h") != std::string::npos;
}

bool engine::Texture::isCompressedFile(const std::string& filename)
{
    return filename.ends_with(".dds") || filename.ends_with(".ktx") || filename.ends_with(".ktx2");
}

bool engine::Texture::isKTX2File(const std::string& filename)
{
	return filename.ends_with(".ktx2");
}

bool engine::Texture::gpuSupportsBC7()
{
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (!extensions) return false;

    // BC7 = BPTC
    return strstr(extensions, "GL_ARB_texture_compression_bptc") != nullptr;
}

GLenum engine::Texture::chooseCompressedFormat(bool isNormal, bool isHeight, bool gamma, bool gpuBC7)
{
    if (isNormal) return GL_COMPRESSED_RG_RGTC2;   // BC5
    if (isHeight) return GL_COMPRESSED_RED_RGTC1;  // BC4

    if (gpuBC7)
        return gamma ? GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
        : GL_COMPRESSED_RGBA_BPTC_UNORM;

    // Fallback to BC3 (DXT5)
    // BC3 is universally supported on all NVIDIA GPUs since 2004
    return gamma ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
}


//engine::Texture::~Texture()
//{
//    // destructor called many many times, don't know why
//    //logger.trace("Texture destructor called");
//
//    // static methods so shouldn't clean anything here
//    //engine::TextureManager::textureIDCache.clear();
//    //engine::TextureManager::textureDataCache.clear();
//    //engine::TextureManager::textureCache.clear();
//}

