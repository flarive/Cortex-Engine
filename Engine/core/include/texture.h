#pragma once

#include "common_defines.h"

#include <vector>

namespace engine
{
    class Texture
    {
    public:
        unsigned int id;
        std::string type;
        std::string path;
		Color color;

        static unsigned int loadImage(std::string filename, bool alpha = false, bool repeat = true, bool gammaCorrection = false);
        static unsigned int loadTexture(std::string filename, bool repeat = true, bool gammaCorrection = false);
        static unsigned int createSolidColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static unsigned int loadCubemap(std::vector<std::string> faces);
    };
}
