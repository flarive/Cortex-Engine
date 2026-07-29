#pragma once

#include "../common_defines.h"

//#define KTX_STATIC
#include "ktx.h"

namespace engine
{
	/// <summary>
	/// Loads Khronos .ktx and .ktx2 files
	/// KTX2 textures are efficient Khronos Texture Container files that store compressed GPU data using Basis Universal formats like ETC1S and UASTC.
	/// They reduce file size and memory use by staying compressed until fast runtime conversion to device-specific hardware formats.
	/// toktx --bcmp --assign_oetf srgb aerial_rocks_04_diff_2k.ktx2 aerial_rocks_04_diff_2k.jpg
	/// toktx --bcmp --assign_oetf linear aerial_rocks_04_nor_gl_2k.ktx2 aerial_rocks_04_nor_gl_2k.jpg
	/// toktx --bcmp --assign_oetf linear aerial_rocks_04_ao_2ko.ktx2 aerial_rocks_04_ao_2k.jpg
	/// toktx --bcmp --assign_oetf linear aerial_rocks_04_rough_2k.ktx2 aerial_rocks_04_rough_2k.jpg
	/// toktx --bcmp --assign_oetf linear aerial_rocks_04_disp_2k.ktx2 aerial_rocks_04_disp_2k.jpg
	/// toktx --bcmp --assign_oetf linear aerial_rocks_04_arm_2k.ktx2 aerial_rocks_04_arm_2k.jpg
	/// </summary>
	class ktxLoader final
	{
	public:
		static GLuint loadAndUploadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap);
		static ktxTexture* loadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap);
		static GLuint uploadKTX_OpenGL(ktxTexture* kTexture);

		static int getKTXComponents(ktxTexture* tex, bool isNormalMap, bool isHeightMap);

	private:

		static ktx_transcode_fmt_e chooseTranscodeFormat(bool isNormalMap, bool isHeightMap);
	};
}