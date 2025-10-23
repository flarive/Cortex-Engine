#pragma once

#include "../common_defines.h"

#include <glm/glm.hpp>

// Color values found at:
// https://www.rapidtables.com/web/color/RGB_Color.html

namespace engine
{
	class Colors final
	{
	public:
		static inline glm::vec4 Maroon = glm::vec4(0.501961f, 0.0f, 0.0f, 1.0f);
		static inline glm::vec4 DarkRed = glm::vec4(0.545098f, 0.0f, 0.0f, 1.0f);
		static inline glm::vec4 Brown = glm::vec4(0.647059f, 0.164706f, 0.164706f, 1.0f);
		static inline glm::vec4 Firebrick = glm::vec4(0.698039f, 0.133333f, 0.133333f, 1.0f);
		static inline glm::vec4 Crimson = glm::vec4(0.862745f, 0.0784314f, 0.235294f, 1.0f);
		static inline glm::vec4 Red = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		static inline glm::vec4 Tomato = glm::vec4(1.0f, 0.388235f, 0.278431f, 1.0f);
		static inline glm::vec4 Coral = glm::vec4(1.0f, 0.498039f, 0.313726f, 1.0f);
		static inline glm::vec4 IndianRed = glm::vec4(0.803922f, 0.360784f, 0.360784f, 1.0f);
		static inline glm::vec4 LightCoral = glm::vec4(0.941176f, 0.501961f, 0.501961f, 1.0f);
		static inline glm::vec4 DarkSalmon = glm::vec4(0.913725f, 0.588235f, 0.478431f, 1.0f);
		static inline glm::vec4 Salmon = glm::vec4(0.980392f, 0.501961f, 0.447059f, 1.0f);
		static inline glm::vec4 LightSalmon = glm::vec4(1.0f, 0.627451f, 0.478431f, 1.0f);
		static inline glm::vec4 OrangeRed = glm::vec4(1.0f, 0.270588f, 0.0f, 1.0f);
		static inline glm::vec4 DarkOrange = glm::vec4(1.0f, 0.54902f, 0.0f, 1.0f);
		static inline glm::vec4 Orange = glm::vec4(1.0f, 0.647059f, 0.0f, 1.0f);
		static inline glm::vec4 Gold = glm::vec4(1.0f, 0.843137f, 0.0f, 1.0f);
		static inline glm::vec4 DarkGoldenRod = glm::vec4(0.721569f, 0.52549f, 0.0431373f, 1.0f);
		static inline glm::vec4 GoldenRod = glm::vec4(0.854902f, 0.647059f, 0.12549f, 1.0f);
		static inline glm::vec4 PaleGoldenRod = glm::vec4(0.933333f, 0.909804f, 0.666667f, 1.0f);
		static inline glm::vec4 DarkKhaki = glm::vec4(0.741176f, 0.717647f, 0.419608f, 1.0f);
		static inline glm::vec4 Khaki = glm::vec4(0.941176f, 0.901961f, 0.54902f, 1.0f);
		static inline glm::vec4 Olive = glm::vec4(0.501961f, 0.501961f, 0.0f, 1.0f);
		static inline glm::vec4 Yellow = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		static inline glm::vec4 YellowGreen = glm::vec4(0.603922f, 0.803922f, 0.196078f, 1.0f);
		static inline glm::vec4 DarkOliveGreen = glm::vec4(0.333333f, 0.419608f, 0.184314f, 1.0f);
		static inline glm::vec4 OliveDrab = glm::vec4(0.419608f, 0.556863f, 0.137255f, 1.0f);
		static inline glm::vec4 LawnGreen = glm::vec4(0.486275f, 0.988235f, 0.0f, 1.0f);
		static inline glm::vec4 ChartReuse = glm::vec4(0.498039f, 1.0f, 0.0f, 1.0f);
		static inline glm::vec4 GreenYellow = glm::vec4(0.678431f, 1.0f, 0.184314f, 1.0f);
		static inline glm::vec4 DarkGreen = glm::vec4(0.0f, 0.392157f, 0.0f, 1.0f);
		static inline glm::vec4 Green = glm::vec4(0.0f, 0.501961f, 0.0f, 1.0f);
		static inline glm::vec4 ForestGreen = glm::vec4(0.133333f, 0.545098f, 0.133333f, 1.0f);
		static inline glm::vec4 Lime = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		static inline glm::vec4 LimeGreen = glm::vec4(0.196078f, 0.803922f, 0.196078f, 1.0f);
		static inline glm::vec4 LightGreen = glm::vec4(0.564706f, 0.933333f, 0.564706f, 1.0f);
		static inline glm::vec4 PaleGreen = glm::vec4(0.596078f, 0.984314f, 0.596078f, 1.0f);
		static inline glm::vec4 DarkSeaGreen = glm::vec4(0.560784f, 0.737255f, 0.560784f, 1.0f);
		static inline glm::vec4 MediumSpringGreen = glm::vec4(0.0f, 0.980392f, 0.603922f, 1.0f);
		static inline glm::vec4 SpringGreen = glm::vec4(0.0f, 1.0f, 0.498039f, 1.0f);
		static inline glm::vec4 SeaGreen = glm::vec4(0.180392f, 0.545098f, 0.341176f, 1.0f);
		static inline glm::vec4 MediumAquaMarine = glm::vec4(0.4f, 0.803922f, 0.666667f, 1.0f);
		static inline glm::vec4 MediumSeaGreen = glm::vec4(0.235294f, 0.701961f, 0.443137f, 1.0f);
		static inline glm::vec4 LightSeaGreen = glm::vec4(0.12549f, 0.698039f, 0.666667f, 1.0f);
		static inline glm::vec4 DarkSlateGray = glm::vec4(0.184314f, 0.309804f, 0.309804f, 1.0f);
		static inline glm::vec4 Teal = glm::vec4(0.0f, 0.501961f, 0.501961f, 1.0f);
		static inline glm::vec4 DarkCyan = glm::vec4(0.0f, 0.545098f, 0.545098f, 1.0f);
		static inline glm::vec4 Aqua = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		static inline glm::vec4 Cyan = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		static inline glm::vec4 LightCyan = glm::vec4(0.878431f, 1.0f, 1.0f, 1.0f);
		static inline glm::vec4 DarkTurquoise = glm::vec4(0.0f, 0.807843f, 0.819608f, 1.0f);
		static inline glm::vec4 Turquoise = glm::vec4(0.25098f, 0.878431f, 0.815686f, 1.0f);
		static inline glm::vec4 MediumTurquoise = glm::vec4(0.282353f, 0.819608f, 0.8f, 1.0f);
		static inline glm::vec4 PaleTurquoise = glm::vec4(0.686275f, 0.933333f, 0.933333f, 1.0f);
		static inline glm::vec4 Aquamarine = glm::vec4(0.498039f, 1.0f, 0.831373f, 1.0f);
		static inline glm::vec4 PowderBlue = glm::vec4(0.690196f, 0.878431f, 0.901961f, 1.0f);
		static inline glm::vec4 CadetBlue = glm::vec4(0.372549f, 0.619608f, 0.627451f, 1.0f);
		static inline glm::vec4 SteelBlue = glm::vec4(0.27451f, 0.509804f, 0.705882f, 1.0f);
		static inline glm::vec4 CornflowerBlue = glm::vec4(0.392157f, 0.584314f, 0.929412f, 1.0f);
		static inline glm::vec4 DeepSkyBlue = glm::vec4(0.0f, 0.74902f, 1.0f, 1.0f);
		static inline glm::vec4 DodgerBlue = glm::vec4(0.117647f, 0.564706f, 1.0f, 1.0f);
		static inline glm::vec4 LightBlue = glm::vec4(0.678431f, 0.847059f, 0.901961f, 1.0f);
		static inline glm::vec4 SkyBlue = glm::vec4(0.529412f, 0.807843f, 0.921569f, 1.0f);
		static inline glm::vec4 LightSkyBlue = glm::vec4(0.529412f, 0.807843f, 0.980392f, 1.0f);
		static inline glm::vec4 MidnightBlue = glm::vec4(0.0980392f, 0.0980392f, 0.439216f, 1.0f);
		static inline glm::vec4 Navy = glm::vec4(0.0f, 0.0f, 0.501961f, 1.0f);
		static inline glm::vec4 DarkBlue = glm::vec4(0.0f, 0.0f, 0.545098f, 1.0f);
		static inline glm::vec4 MediumBlue = glm::vec4(0.0f, 0.0f, 0.803922f, 1.0f);
		static inline glm::vec4 Blue = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		static inline glm::vec4 RoyalBlue = glm::vec4(0.254902f, 0.411765f, 0.882353f, 1.0f);
		static inline glm::vec4 BlueViolet = glm::vec4(0.541176f, 0.168627f, 0.886275f, 1.0f);
		static inline glm::vec4 Indigo = glm::vec4(0.294118f, 0.0f, 0.509804f, 1.0f);
		static inline glm::vec4 DarkSlateBlue = glm::vec4(0.282353f, 0.239216f, 0.545098f, 1.0f);
		static inline glm::vec4 SlateBlue = glm::vec4(0.415686f, 0.352941f, 0.803922f, 1.0f);
		static inline glm::vec4 MediumSlateBlue = glm::vec4(0.482353f, 0.407843f, 0.933333f, 1.0f);
		static inline glm::vec4 MediumPurple = glm::vec4(0.576471f, 0.439216f, 0.858824f, 1.0f);
		static inline glm::vec4 DarkMagenta = glm::vec4(0.545098f, 0.0f, 0.545098f, 1.0f);
		static inline glm::vec4 DarkViolet = glm::vec4(0.580392f, 0.0f, 0.827451f, 1.0f);
		static inline glm::vec4 DarkOrchid = glm::vec4(0.6f, 0.196078f, 0.8f, 1.0f);
		static inline glm::vec4 MediumOrchid = glm::vec4(0.729412f, 0.333333f, 0.827451f, 1.0f);
		static inline glm::vec4 Purple = glm::vec4(0.501961f, 0.0f, 0.501961f, 1.0f);
		static inline glm::vec4 Thistle = glm::vec4(0.847059f, 0.74902f, 0.847059f, 1.0f);
		static inline glm::vec4 Plum = glm::vec4(0.866667f, 0.627451f, 0.866667f, 1.0f);
		static inline glm::vec4 Violet = glm::vec4(0.933333f, 0.509804f, 0.933333f, 1.0f);
		static inline glm::vec4 Magenta = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		static inline glm::vec4 Orchid = glm::vec4(0.854902f, 0.439216f, 0.839216f, 1.0f);
		static inline glm::vec4 MediumVioletRed = glm::vec4(0.780392f, 0.0823529f, 0.521569f, 1.0f);
		static inline glm::vec4 PaleVioletRed = glm::vec4(0.858824f, 0.439216f, 0.576471f, 1.0f);
		static inline glm::vec4 DeepPink = glm::vec4(1.0f, 0.0784314f, 0.576471f, 1.0f);
		static inline glm::vec4 HotPink = glm::vec4(1.0f, 0.411765f, 0.705882f, 1.0f);
		static inline glm::vec4 LightPink = glm::vec4(1.0f, 0.713726f, 0.756863f, 1.0f);
		static inline glm::vec4 Pink = glm::vec4(1.0f, 0.752941f, 0.796078f, 1.0f);
		static inline glm::vec4 AntiqueWhite = glm::vec4(0.980392f, 0.921569f, 0.843137f, 1.0f);
		static inline glm::vec4 Beige = glm::vec4(0.960784f, 0.960784f, 0.862745f, 1.0f);
		static inline glm::vec4 Bisque = glm::vec4(1.0f, 0.894118f, 0.768627f, 1.0f);
		static inline glm::vec4 BlanchedAlmond = glm::vec4(1.0f, 0.921569f, 0.803922f, 1.0f);
		static inline glm::vec4 Wheat = glm::vec4(0.960784f, 0.870588f, 0.701961f, 1.0f);
		static inline glm::vec4 CornSilk = glm::vec4(1.0f, 0.972549f, 0.862745f, 1.0f);
		static inline glm::vec4 LemonChiffon = glm::vec4(1.0f, 0.980392f, 0.803922f, 1.0f);
		static inline glm::vec4 LightGoldenRodYellow = glm::vec4(0.980392f, 0.980392f, 0.823529f, 1.0f);
		static inline glm::vec4 LightYellow = glm::vec4(1.0f, 1.0f, 0.878431f, 1.0f);
		static inline glm::vec4 SaddleBrown = glm::vec4(0.545098f, 0.270588f, 0.0745098f, 1.0f);
		static inline glm::vec4 Sienna = glm::vec4(0.627451f, 0.321569f, 0.176471f, 1.0f);
		static inline glm::vec4 Chocolate = glm::vec4(0.823529f, 0.411765f, 0.117647f, 1.0f);
		static inline glm::vec4 Peru = glm::vec4(0.803922f, 0.521569f, 0.247059f, 1.0f);
		static inline glm::vec4 SandyBrown = glm::vec4(0.956863f, 0.643137f, 0.376471f, 1.0f);
		static inline glm::vec4 BurlyWood = glm::vec4(0.870588f, 0.721569f, 0.529412f, 1.0f);
		static inline glm::vec4 Tan = glm::vec4(0.823529f, 0.705882f, 0.54902f, 1.0f);
		static inline glm::vec4 RosyBrown = glm::vec4(0.737255f, 0.560784f, 0.560784f, 1.0f);
		static inline glm::vec4 Moccasin = glm::vec4(1.0f, 0.894118f, 0.709804f, 1.0f);
		static inline glm::vec4 NavajoWhite = glm::vec4(1.0f, 0.870588f, 0.678431f, 1.0f);
		static inline glm::vec4 PeachPuff = glm::vec4(1.0f, 0.854902f, 0.72549f, 1.0f);
		static inline glm::vec4 MistyRose = glm::vec4(1.0f, 0.894118f, 0.882353f, 1.0f);
		static inline glm::vec4 LavenderBlush = glm::vec4(1.0f, 0.941176f, 0.960784f, 1.0f);
		static inline glm::vec4 Linen = glm::vec4(0.980392f, 0.941176f, 0.901961f, 1.0f);
		static inline glm::vec4 OldLace = glm::vec4(0.992157f, 0.960784f, 0.901961f, 1.0f);
		static inline glm::vec4 PapayaWhip = glm::vec4(1.0f, 0.937255f, 0.835294f, 1.0f);
		static inline glm::vec4 SeaShell = glm::vec4(1.0f, 0.960784f, 0.933333f, 1.0f);
		static inline glm::vec4 MintCream = glm::vec4(0.960784f, 1.0f, 0.980392f, 1.0f);
		static inline glm::vec4 SlateGray = glm::vec4(0.439216f, 0.501961f, 0.564706f, 1.0f);
		static inline glm::vec4 LightSlateGray = glm::vec4(0.466667f, 0.533333f, 0.6f, 1.0f);
		static inline glm::vec4 LightSteelBlue = glm::vec4(0.690196f, 0.768627f, 0.870588f, 1.0f);
		static inline glm::vec4 Lavender = glm::vec4(0.901961f, 0.901961f, 0.980392f, 1.0f);
		static inline glm::vec4 FloralWhite = glm::vec4(1.0f, 0.980392f, 0.941176f, 1.0f);
		static inline glm::vec4 AliceBlue = glm::vec4(0.941176f, 0.972549f, 1.0f, 1.0f);
		static inline glm::vec4 GhostWhite = glm::vec4(0.972549f, 0.972549f, 1.0f, 1.0f);
		static inline glm::vec4 Honeydew = glm::vec4(0.941176f, 1.0f, 0.941176f, 1.0f);
		static inline glm::vec4 Ivory = glm::vec4(1.0f, 1.0f, 0.941176f, 1.0f);
		static inline glm::vec4 Azure = glm::vec4(0.941176f, 1.0f, 1.0f, 1.0f);
		static inline glm::vec4 Snow = glm::vec4(1.0f, 0.980392f, 0.980392f, 1.0f);
		static inline glm::vec4 Black = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		static inline glm::vec4 DimGrey = glm::vec4(0.411765f, 0.411765f, 0.411765f, 1.0f);
		static inline glm::vec4 Grey = glm::vec4(0.501961f, 0.501961f, 0.501961f, 1.0f);
		static inline glm::vec4 DarkGrey = glm::vec4(0.662745f, 0.662745f, 0.662745f, 1.0f);
		static inline glm::vec4 Silver = glm::vec4(0.752941f, 0.752941f, 0.752941f, 1.0f);
		static inline glm::vec4 LightGrey = glm::vec4(0.827451f, 0.827451f, 0.827451f, 1.0f);
		static inline glm::vec4 Gainsboro = glm::vec4(0.862745f, 0.862745f, 0.862745f, 1.0f);
		static inline glm::vec4 WhiteSmoke = glm::vec4(0.960784f, 0.960784f, 0.960784f, 1.0f);
		static inline glm::vec4 White = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Converts a hex string like "#FF5733" or "FF5733" to normalized RGB
		static Color hexToNormalizedRGB(const std::string& hex)
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

			return Color{
				r / 255.0f,
				g / 255.0f,
				b / 255.0f,
				1.0f
			};
		}
	};
}
