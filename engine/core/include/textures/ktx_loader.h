#pragma once

#include "../common_defines.h"

//#define KTX_STATIC
#include "ktx.h"



namespace engine
{
	class ktxLoader final
	{
	public:
		static GLuint loadAndUploadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap);
		static ktxTexture* loadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap);
		static GLuint uploadKTX_OpenGL(ktxTexture* kTexture);

	private:

		static ktx_transcode_fmt_e chooseTranscodeFormat(bool isNormalMap, bool isHeightMap);
	};
}

