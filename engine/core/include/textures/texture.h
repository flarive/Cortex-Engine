#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include "ktx.h"

#include <future>
#include <unordered_map>
#include <mutex>
#include <queue>



// not added by GLAD ext GL_EXT_texture_compression_s3tc so we define them here to avoid compilation errors
#ifndef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT        0x8C4C
#endif

#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT  0x8C4E
#endif

#ifndef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT  0x8C4F
#endif


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




    
    using TextureData = std::tuple<unsigned int, unsigned char*, int, int, int>;

    struct TextureLoadResult final {
        std::future<TexturePayload> future;
        bool ready = false;
        TexturePayload result;   // <-- FIXED
    };

    
    namespace TextureManager {
        extern std::unordered_map<std::string, TextureLoadResult> textureCache;
        extern std::queue<std::function<void()>> textureUploadQueue;
        extern std::mutex textureCacheMutex;
        extern std::mutex textureQueueMutex;
        extern std::unordered_map<std::string, unsigned int> textureIDCache;
    }

    
    enum TextureFlags : uint32_t
    {
        TextureFlag_None = 0,
        TextureFlag_InvertY = 1 << 0,
        TextureFlag_GenerateMipmaps = 1 << 1,
        TextureFlag_RepeatTexture = 1 << 2,
        TextureFlag_GammaCorrect = 1 << 3
    };

    inline TextureFlags operator|(TextureFlags a, TextureFlags b)
    {
        return static_cast<TextureFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline TextureFlags operator&(TextureFlags a, TextureFlags b)
    {
        return static_cast<TextureFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }


    inline bool hasFlag(TextureFlags flags, TextureFlags flag)
    {
        return (flags & flag) != 0;
    }


    /// <summary>
    /// PNG/JPG → small file on disk
    /// BC7/BC5/BC4 → small texture in VRAM
    /// </summary>
    class Texture final : private NonCopyableButMovable
    {
    public:
        unsigned int id{};
        std::string type{};
        std::string path{};
        Color color{};

		Texture() = default;
        Texture(unsigned int id, const std::string& type, const std::string& path);
		~Texture() = default;

        // override a little NonCopyableButMovable
        Texture(Texture&&) = default;

        void bind() const;

        static unsigned int loadTexture(const std::string& filename, TextureFlags flags = TextureFlag_None);
        
        static TextureData loadTextureExtended(const std::string& filename, TextureFlags flags = TextureFlag_None);
        
        
        static unsigned int createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static unsigned int loadCubemap(const std::vector<std::string>& faces);
        static unsigned int loadHDRImage(const std::string& filename, bool alpha = false, TextureFlags flags = TextureFlag_RepeatTexture);

        static unsigned int requestLoadTextureAsync(const std::string& filename);
        static unsigned int enqueueAsyncTextureCreation(const std::string& filename, TextureFlags flags = TextureFlag_InvertY | TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
        static void processLoadedTextures();
        static unsigned int createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool isCompressed, bool isNormalMap, bool isHeightMap, TextureFlags flags);

        static unsigned int loadTextureFromFile(const char* path, const std::string& directory, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);
        static unsigned int loadGLTextureFromFile(const char* path, const std::string& directory, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);
        static unsigned int loadTextureFromMemory(const unsigned char* data, size_t size, const char* filename, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);
        static unsigned int loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height, TextureFlags flags = TextureFlag_RepeatTexture | TextureFlag_GenerateMipmaps);

        static GLuint loadMTexture();
        static GLuint loadLUTTexture();

        static void checkTextureIsValid(unsigned int textureID);

        static engine::TextureData getTextureData(const std::string& texturePath);

    private:

        static unsigned char* flipImageVertically(unsigned char* data, int width, int height, int nrComponents);
        static void flipImageVertically2(unsigned char* data, int width, int height, int nrComponents);

        static bool isNormalMap(const std::string& filename);
        static bool isHeightMap(const std::string& filename);

        static unsigned int createUncompressedTexture(const std::string& filename, TextureFlags flags);
        static unsigned int createCompressedTexture(const std::string& filename, TextureFlags flags);

        static bool isCompressedFile(const std::string& filename);
        static bool isKTX2File(const std::string& filename);
        static bool gpuSupportsBC7();
        static GLenum chooseCompressedFormat(bool isNormal, bool isHeight, bool gamma, bool gpuBC7);


    };
}
