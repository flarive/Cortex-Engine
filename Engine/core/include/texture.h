#pragma once

#include "misc/noncopyable.h"
#include "common_defines.h"

#include <vector>

namespace engine
{
	class Texture// : private NonCopyable
    {
    public:
        unsigned int id{};
        std::string type{};
        std::string path{};
        Color color{};

		Texture() = default;
		~Texture() = default;

        static unsigned int loadTexture(const std::string& filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static unsigned int loadCubemap(const std::vector<std::string>& faces);
        static unsigned int loadHDRImage(const std::string& filename, bool alpha = false, bool repeat = true);
    };
}
