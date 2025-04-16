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
    namespace TextureManager {
        extern std::unordered_map<std::string, std::future<std::tuple<unsigned char*, int, int, int>>> textureCache;
        extern std::queue<std::function<void()>> textureUploadQueue;
        extern std::mutex textureCacheMutex;
        extern std::mutex textureQueueMutex;
        extern std::unordered_map<std::string, unsigned int> textureIDCache;
    }

    
    class Texture// : private NonCopyable
    {
    public:
        unsigned int id{};
        std::string type{};
        std::string path{};
        Color color{};

		Texture() = default;
        Texture(unsigned int id, const std::string& type, const std::string& path);
		~Texture() = default;

        static std::tuple<unsigned int, unsigned char*, int, int, int> loadTextureExtended(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int loadTexture(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int loadTextureAsync(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static unsigned int loadCubemap(const std::vector<std::string>& faces);
        static unsigned int loadHDRImage(const std::string& filename, bool alpha = false, bool repeat = true);


        static std::tuple<unsigned char*, int, int, int> loadTextureAsyncInternal(const std::string& filename);
        static void processLoadedTextures();
        static unsigned int enqueueTextureCreation(const std::string& filename, bool generateMipmaps = true, bool repeat = false, bool gammaCorrection = false);

        static unsigned int createOpenGLTexture(unsigned char* data, int width, int height, int nrComponents, bool generateMipmaps, bool repeat, bool gammaCorrection);
    };
}
