#pragma once

#include "../common_defines.h"

#include "../textures/texture.h"

#include "ktx.h"

#include <future>
#include <unordered_map>
#include <mutex>
#include <queue>

namespace engine
{
    enum class TextureSourceType {
        RawPixels,   // SOIL, PNG, JPG, DDS
        KTXTexture   // KTX1 / KTX2
    };

    struct TexturePayload {
        TextureSourceType type;

        // Raw pixels (SOIL)
        unsigned char* rawData = nullptr;

        // KTX texture object
        ktxTexture* ktxData = nullptr;

        int width = 0;
        int height = 0;
        int components = 0;
    };


    struct TextureData {
        GLuint id;
        unsigned char* data;
        std::string filePath;
        int width;
        int height;
        int nbComponents;
        ubyte thumbnailLevel;
    };


    struct TextureLoadResult final {
        std::future<TexturePayload> future;
        bool ready = false;
        TexturePayload result;
    };

    
    struct TextureUploadResult final {
        GLuint textureID;
        int width;
        int height;
        int nbComponents;
        ubyte thumbnailLevel;
    };


    namespace TextureManagerInternal {
        extern std::unordered_map<std::string, TextureLoadResult> asyncLoadingTextureCache;
        extern std::queue<std::function<void()>> textureUploadQueue;
        extern std::mutex textureCacheMutex;
        extern std::mutex textureQueueMutex;
        extern std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache; // key must be the texture full absolute path
        extern std::unordered_map<std::string, TextureData> textureDataCache; // key must be the texture full absolute path
    }


    const int TARGET_THUMB_SIZE = 32;

    /// <summary>
    /// TextureManager is a static manager for the moment
    /// </summary>
    class TextureManager final
	{
	public:
        // Synchronous texture loading (shouldn't be used anymore, prefer loadTextureAsync)
        static std::shared_ptr<Texture> loadTexture(const std::string& path, const TextureSlot& type, TextureFlags flags = TextureFlag_None);
        static TextureData loadAndUploadTextureExtended(const std::string& path, TextureFlags flags = TextureFlag_None);


        static GLuint createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static GLuint loadCubemap(const std::vector<std::string>& faces);
        static GLuint loadHDRImage(const std::string& path, bool alpha = false, TextureFlags flags = TextureFlag_RepeatTexture);

        // async textures loading
        static void requestLoadTextureAsync(TextureSlot slot, const std::string& path, bool flipY = false);
        static void enqueueAsyncTextureCreation(const std::string& path, const TextureSlot& type, TextureFlags flags = TextureFlag_InvertY | TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
        static void processLoadedTextures();
        static TextureUploadResult createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, TextureFlags flags);

        static TexturePayload loadTextureFromFile(const std::string& path, TextureFlags flags = TextureFlag_None);

        static TextureUploadResult loadTextureFromMemory(const unsigned char* data, size_t size, const char* path, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);
        static TextureUploadResult loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);

        static GLuint loadMTexture();
        static GLuint loadLUTTexture();

        static void checkTextureIsValid(GLuint textureID);
        
        static const TextureData* getTextureData(const std::string& texturePath);


        static const std::shared_ptr<Texture> getTextureFromCache(const std::string& path);
        

    private:
        static void flipImageVertically2(unsigned char* data, int width, int height, int nrComponents);

        static bool isNormalMap(const std::string& filename);
        static bool isHeightMap(const std::string& filename);

        static bool isCompressedFile(const std::string& filename);
        static bool isDDSFile(const std::string& filename);
        static bool isKTXFile(const std::string& filename);
        static bool isKTX2File(const std::string& filename);
	};
}