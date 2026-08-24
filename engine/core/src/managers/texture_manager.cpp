#include "../../include/managers/texture_manager.h"

#include "../../include/managers/log_manager.h"
#include "../../include/managers/filesystem_manager.h"

#include "../../include/misc/ltc_matrix.h"

#include "../../include/textures/ktx_loader.h"


#include "SOIL2.h"

#include <iostream>
#include <functional>


namespace engine {
    namespace TextureManagerInternal {
        std::unordered_map<std::string, TextureLoadResult> asyncLoadingTextureCache;
        std::queue<std::function<void()>> textureUploadQueue;
        std::mutex textureCacheMutex;
        std::mutex textureQueueMutex;
        std::unordered_map<std::string, unsigned int> textureIDCache; // key must be the texture full absolute path (could be removed may be ?)
        std::unordered_map<std::string, TextureData> textureDataCache; // key must be the texture full absolute path
    }
}

/// <summary>
/// Synchronous texture loading (shouldn't be used, prefer loadTextureAsync)
/// </summary>
unsigned int engine::TextureManager::loadTexture(const std::string& filename, TextureFlags flags)
{
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    // Detect normal maps by filename
    bool isNormalMap = TextureManager::isNormalMap(filename);

    // Detect heightmaps
    bool isHeightMap = TextureManager::isHeightMap(filename);


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
/// Synchronous texture loading with extended result POUBELLE !!!!!!!!!!!!!!!!!!!!!!!!
/// </summary>
engine::TextureData engine::TextureManager::loadAndUploadTextureExtended(const std::string& filename, TextureFlags flags)
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

    return { textureID, data, filename, width, height, nrComponents, 0 };
}

/// <summary>
/// Asynchronous texture loading
/// </summary>
unsigned int engine::TextureManager::requestLoadTextureAsync(const std::string& path)
{
    if (path.empty()) return 0;

    std::lock_guard<std::mutex> lock(engine::TextureManagerInternal::textureCacheMutex);

    // Check if the texture is already being loaded asynchronously
    if (engine::TextureManagerInternal::asyncLoadingTextureCache.find(path) != engine::TextureManagerInternal::asyncLoadingTextureCache.end()) {
        return 0; // Already loading
    }

    logger.info("Loading async texture {}", FileSystemManager::getShortenedPath(path));

    engine::TextureManagerInternal::asyncLoadingTextureCache[path] =
    {
        std::async(std::launch::async, [path]() -> TexturePayload
        {
            TexturePayload payload;

            if (isKTX2File(path))
            {
                // compressed texture, use ktx loader
                // ktx lib can load ktx and ktx2
                payload.type = TextureSourceType::KTXTexture;

                payload.ktxData = ktxLoader::loadKTX(
                    path,
                    isNormalMap(path),
                    isHeightMap(path)
                );

                if (!payload.ktxData)
                    return payload;

                payload.width = payload.ktxData->baseWidth;
                payload.height = payload.ktxData->baseHeight;
                payload.components = ktxLoader::getKTXComponents(payload.ktxData, isNormalMap(path), isHeightMap(path));

                return payload;
            }
            else
            {
                // uncompressed texture, use SOIL loader
                // SOIL can load jpg, png, dds...
                payload.type = TextureSourceType::RawPixels;

                if (path.c_str()[path.size() - 2] == '*')
                {
                    // Embedded texture from model (already loaded in memory), retrieve it from cache
                    const TextureData* tex = getTextureData(path); // direct use of TexturePayload ?????????????????????
                    if (tex)
                    {
                        payload.rawData = tex->data;
                        payload.width = tex->width;
                        payload.height = tex->height;
                        payload.components = tex->nbComponents;
                    }
                }
                else
                {
                    // External texture file
                    payload = loadTextureFromFile(path, TextureFlag_GenerateMipmaps | TextureFlag_InvertY);
                }

                return payload;
            }
        }),
        false,
        {}
    };

    return 0;  // Temporary ID, real ID is set later
}

//unsigned char* engine::TextureManager::flipImageVertically(unsigned char* data, int width, int height, int nrComponents)
//{
//    unsigned char* flippedData = new unsigned char[width * height * nrComponents];
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            for (int c = 0; c < nrComponents; c++) {
//                flippedData[(height - 1 - y) * width * nrComponents + x * nrComponents + c] =
//                    data[y * width * nrComponents + x * nrComponents + c];
//            }
//        }
//    }
//    delete[] data;
//    return flippedData;
//}

void engine::TextureManager::flipImageVertically2(unsigned char* data, int width, int height, int nrComponents)
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
void engine::TextureManager::processLoadedTextures()
{
    std::lock_guard<std::mutex> lock(engine::TextureManagerInternal::textureQueueMutex);

    while (!engine::TextureManagerInternal::textureUploadQueue.empty())
    {
        engine::TextureManagerInternal::textureUploadQueue.front()(); // Execute OpenGL task
        engine::TextureManagerInternal::textureUploadQueue.pop();
    }
}

/// <summary>
/// Enqueue Texture Creation to Run on Main Thread
/// </summary>
unsigned int engine::TextureManager::enqueueAsyncTextureCreation(const std::string& path, TextureFlags flags)
{
    std::lock_guard<std::mutex> lock(engine::TextureManagerInternal::textureCacheMutex);

    bool isNormalMap = TextureManager::isNormalMap(path);
    bool isHeightMap = TextureManager::isHeightMap(path);
    bool isCompressed = isCompressedFile(path);

    // 1. Check async load result exists
    auto it = engine::TextureManagerInternal::asyncLoadingTextureCache.find(path);
    if (it == engine::TextureManagerInternal::asyncLoadingTextureCache.end())
    {
        logger.warn("Texture future for {} not found !", FileSystemManager::getShortenedPath(path));
        return 0;
    }

    auto& entry = it->second;

    // 2. Retrieve future result once
    if (!entry.ready)
    {
        if (!entry.future.valid())
        {
            logger.warn("Texture future for {} is invalid !", FileSystemManager::getShortenedPath(path));
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
            logger.error("Raw texture {} failed to load !", FileSystemManager::getShortenedPath(path));
            return 0;
        }
    }
    else if (payload.type == TextureSourceType::KTXTexture)
    {
        if (!payload.ktxData)
        {
            logger.error("KTX texture {} failed to load !", FileSystemManager::getShortenedPath(path));
            return 0;
        }
    }

    // 4. Avoid duplicate uploads
    if (engine::TextureManagerInternal::textureIDCache.contains(path))
        return engine::TextureManagerInternal::textureIDCache[path];

    // 5. Enqueue OpenGL upload on main thread
    {
        std::lock_guard<std::mutex> lock(engine::TextureManagerInternal::textureQueueMutex);

        engine::TextureManagerInternal::textureUploadQueue.push(
            [path, payload, isCompressed, isNormalMap, isHeightMap, flags]()
            {
                TextureUploadResult uploadResult{};

                if (payload.type == TextureSourceType::RawPixels)
                {
                    uploadResult = createOpenGLTexture(
                        payload.rawData,
                        payload.width,
                        payload.height,
                        payload.components,
                        isCompressed,
                        isNormalMap,
                        isHeightMap,
                        flags
                    );

					// texture uploaded to GPU, now we can free CPU memory
                    SOIL_free_image_data(payload.rawData);
                }
                else if (payload.type == TextureSourceType::KTXTexture)
                {
                    uploadResult = ktxLoader::uploadKTX_OpenGL(payload.ktxData);

                    ktxTexture_Destroy(payload.ktxData);
                }

                // Cache result
                engine::TextureManagerInternal::textureIDCache[path] = uploadResult.textureID;
                engine::TextureManagerInternal::textureDataCache[path] = TextureData{ uploadResult.textureID, nullptr, path, payload.width, payload.height, payload.components, uploadResult.thumbnailLevel};

                return uploadResult.textureID;
            }
        );
    }

    return 0;
}

/// <summary>
/// Creates OpenGL Texture and upload it on the GPU (Always Called on Main Thread)
/// </summary>
engine::TextureUploadResult engine::TextureManager::createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool isCompressed, bool isNormalMap, bool isHeightMap, TextureFlags flags)
{
    TextureUploadResult result{};
    result.nbComponents = nrComponents;
    result.width = width;
    result.height = height;

    if (!data)
        return result;

    // Flip vertically if requested
    //if (hasFlag(flags, TextureFlag_InvertY))
    //    flipImageVertically2(data, width, height, nrComponents);

    // Create and bind OpenGL texture
    unsigned int textureID{};
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    result.textureID = textureID;


    GLenum externalFormat{};
    if (nrComponents == 1) externalFormat = GL_RED;
    else if (nrComponents == 3) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB : GL_RGB;
    else if (nrComponents == 4) externalFormat = hasFlag(flags, TextureFlag_GammaCorrect) ? GL_SRGB_ALPHA : GL_RGBA;

    // Choose compressed internal format
    GLenum internalFormat{};
    if (isCompressed)
        internalFormat = chooseCompressedFormat(isNormalMap, isHeightMap, hasFlag(flags, TextureFlag_GammaCorrect), gpuSupportsBC7());

    // Upload texture to GPU with or without compression
    glTexImage2D(GL_TEXTURE_2D, 0, isCompressed ? internalFormat : externalFormat, width, height, 0, externalFormat, GL_UNSIGNED_BYTE, data);

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

    
    // Compute thumbnail mip level (target thumbnail size = 64x64)
    int level = 0;
    int w = width;

    while (w > TARGET_THUMB_SIZE)
    {
        w >>= 1;   // divide by 2
        level++;
    }

    result.thumbnailLevel = level;

    // Clamp mipmap chain so levels below thumbnail are not generated
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);

    // wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

    return result;
}

/// <summary>
/// Uncompressed texture (PNG/JPG/TGA/HDR)
/// Small texture size on disk but using more VRAM on GPU
/// </summary>
/// <param name="filename"></param>
/// <param name="flags"></param>
/// <returns></returns>
unsigned int engine::TextureManager::createUncompressedTexture(const std::string& filename, TextureFlags flags)
{
    // TO DO !
    return 0;
}

/// <summary>
/// Compressed texture (DDS/KTX2 (BC7 / BC5 / BC4))
/// Bigger size on disk but using less VRAM on GPU
/// </summary>
/// <param name="filename"></param>
/// <param name="flags"></param>
/// <returns></returns>
unsigned int engine::TextureManager::createCompressedTexture(const std::string& filename, TextureFlags flags)
{
    // TO DO !
    return 0;
}

unsigned int engine::TextureManager::createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
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

unsigned int engine::TextureManager::loadCubemap(const std::vector<std::string>& faces)
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

unsigned int engine::TextureManager::loadHDRImage(const std::string& filename, bool alpha, TextureFlags flags)
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

engine::TexturePayload engine::TextureManager::loadTextureFromFile(const std::string& path, TextureFlags flags)
{
    logger.info("Loading texture {}", FileSystemManager::getShortenedPath(path));
    
    int width{}, height{}, nrComponents{};
    unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

    if (hasFlag(flags, TextureFlag_InvertY))
        flipImageVertically2(data, width, height, nrComponents);

    // SOIL_free_image_data muts be called by caller after the texture is uploaded to GPU, otherwise the texture data will be lost and the texture will be black
    return TexturePayload{ TextureSourceType::RawPixels, data, nullptr, width, height, nrComponents };
}

unsigned int engine::TextureManager::loadAndUploadTextureFromFile(const std::string& path, TextureFlags flags)
{
    logger.info("Loading texture {}", FileSystemManager::getShortenedPath(path));

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width = 0, height = 0, nrComponents = 0;
    unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &nrComponents, SOIL_LOAD_AUTO);

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


        // Compute thumbnail mip level (target thumbnail size = 64x64)
        int level = 0;
        int w = width;

        while (w > TARGET_THUMB_SIZE)
        {
            w >>= 1;   // divide by 2
            level++;
        }

        ubyte thumbnailLevel = level;

        // Clamp mipmap chain so levels below thumbnail are not generated
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, hasFlag(flags, TextureFlag_RepeatTexture) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

        // filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, hasFlag(flags, TextureFlag_GenerateMipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);


        // Cache result (bof bof)
        engine::TextureManagerInternal::textureIDCache[path] = textureID;
        engine::TextureManagerInternal::textureDataCache[path] = TextureData{ textureID, data, path, width, height, nrComponents, thumbnailLevel };


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

engine::TextureUploadResult engine::TextureManager::loadTextureFromMemory(const unsigned char* data, size_t size, const char* filename, TextureFlags flags)
{
    int width = 0, height = 0, channels = 0;

    logger.info("Loading memory texture {}", filename);

    // Load image from memory buffer using SOIL
    unsigned char* image = SOIL_load_image_from_memory(data, static_cast<int>(size), &width, &height, &channels, SOIL_LOAD_AUTO);

    if (hasFlag(flags, TextureFlag_InvertY))
        flipImageVertically2(image, width, height, channels);

    if (!image)
    {
        std::cerr << "Failed to load embedded texture from memory." << std::endl;
        return TextureUploadResult{};
    }

    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    ubyte thumbnailLevel = 0; // ?????????????????????????

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

    return TextureUploadResult{ textureID, width, height, channels, thumbnailLevel };
}

engine::TextureUploadResult engine::TextureManager::loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height, TextureFlags flags)
{
    if (!data || height == 0 || width == 0)
    {
        std::cerr << "Invalid uncompressed texture." << std::endl;
        return TextureUploadResult{};
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

    int channels = 4; // aiTexel is always RGBA
    ubyte thumbnailLevel = 0; // ?????????????????????????

    return TextureUploadResult{ textureID, (int)width, (int)height, channels, thumbnailLevel };
}

GLuint engine::TextureManager::loadMTexture()
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

GLuint engine::TextureManager::loadLUTTexture()
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

void engine::TextureManager::checkTextureIsValid(unsigned int textureID)
{
    // After loading the textures, check if they are valid
    GLint width2, height2, internalFormat;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width2);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height2);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
    std::cout << "textureID: " << textureID << ", width: " << width2 << ", height: " << height2 << ", format: " << internalFormat << std::endl;
}

// Best practice: avoid copying large texture data
const engine::TextureData* engine::TextureManager::getTextureData(const std::string& texturePath)
{
    auto it = engine::TextureManagerInternal::textureDataCache.find(texturePath);
    if (it != engine::TextureManagerInternal::textureDataCache.end())
        return &it->second;

    return nullptr;
}

bool engine::TextureManager::isNormalMap(const std::string& filename)
{
    return filename.find("normal") != std::string::npos || filename.find("_n") != std::string::npos || filename.find("_norm") != std::string::npos;
}

bool engine::TextureManager::isHeightMap(const std::string& filename)
{
    return filename.find("height") != std::string::npos || filename.find("_h") != std::string::npos;
}

bool engine::TextureManager::isCompressedFile(const std::string& filename)
{
    return isDDSFile(filename) || isKTXFile(filename) || isKTX2File(filename);
}

bool engine::TextureManager::isDDSFile(const std::string& filename)
{
    return filename.ends_with(".dds");
}

bool engine::TextureManager::isKTXFile(const std::string& filename)
{
    return filename.ends_with(".ktx");
}

bool engine::TextureManager::isKTX2File(const std::string& filename)
{
    return filename.ends_with(".ktx2");
}

bool engine::TextureManager::gpuSupportsBC7()
{
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (!extensions) return false;

    // BC7 = BPTC
    return strstr(extensions, "GL_ARB_texture_compression_bptc") != nullptr;
}

GLenum engine::TextureManager::chooseCompressedFormat(bool isNormal, bool isHeight, bool gamma, bool gpuBC7)
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

//void engine::TextureManager::loadFromMemory(unsigned char* data, int size)
//{
//    int w, h, comp;
//    unsigned char* decoded = SOIL_load_image_from_memory(
//        data,
//        size,   // byte size
//        &w, &h, &comp,
//        SOIL_LOAD_AUTO
//    );
//
//}
