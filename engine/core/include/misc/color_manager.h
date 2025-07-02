#include <string>
#include <stdexcept>

#include "../common_defines.h"

namespace engine
{
    class ColorManager
    {
    public:
        // Converts a hex string like "#FF5733" or "FF5733" to normalized RGB
        static engine::Color hexToNormalizedRGB(const std::string& hex)
        {
            std::string cleanHex = hex;
            if (cleanHex[0] == '#') {
                cleanHex = cleanHex.substr(1);
            }

            if (cleanHex.length() != 6) {
                throw std::invalid_argument("Hex color must be 6 characters long.");
            }

            int r = std::stoi(cleanHex.substr(0, 2), nullptr, 16);
            int g = std::stoi(cleanHex.substr(2, 2), nullptr, 16);
            int b = std::stoi(cleanHex.substr(4, 2), nullptr, 16);

            return engine::Color{
                r / 255.0f,
                g / 255.0f,
                b / 255.0f,
                1.0f
            };
        }
    };
}
