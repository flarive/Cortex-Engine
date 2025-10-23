#pragma once

#include "misc/noncopyable.h"
#include "common_defines.h"

#include <vector>
#include <future>
#include <unordered_map>
#include <mutex>
#include <queue>

namespace engine
{
    struct TextureLoadResult final {
        std::future<std::tuple<unsigned char*, int, int, int>> future;
        bool ready = false;
        std::tuple<unsigned char*, int, int, int> result;
    };
    
    namespace TextureManager {
        extern std::unordered_map<std::string, TextureLoadResult> textureCache;
        extern std::queue<std::function<void()>> textureUploadQueue;
        extern std::mutex textureCacheMutex;
        extern std::mutex textureQueueMutex;
        extern std::unordered_map<std::string, unsigned int> textureIDCache;
    }

    
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

        static std::tuple<unsigned int, unsigned char*, int, int, int> loadTextureExtended(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int loadTexture(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int loadTextureAsync(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static unsigned int loadCubemap(const std::vector<std::string>& faces);
        static unsigned int loadHDRImage(const std::string& filename, bool alpha = false, bool repeat = true);


        static void processLoadedTextures();
        static unsigned int enqueueTextureCreation(const std::string& filename, bool generateMipmaps = true, bool repeat = false, bool gammaCorrection = false);

        static unsigned int createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool generateMipmaps, bool repeat, bool gammaCorrection);

        static unsigned int loadTextureFromFile(const char* path, const std::string& directory);
        static unsigned int loadGLTextureFromFile(const char* path, const std::string& directory);
        static unsigned int loadTextureFromMemory(const unsigned char* data, size_t size, const char* filename);
        static unsigned int loadUncompressedTexture(const unsigned char* data, unsigned int width, unsigned int height);

        static GLuint loadMTexture();
        static GLuint loadLUTTexture();

        static void CheckTextureIsValid(unsigned int textureID);
    };
}
