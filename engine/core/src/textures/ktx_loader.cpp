#include "../../include/textures/ktx_loader.h"

GLuint engine::ktxLoader::loadAndUploadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap)
{
    ktxTexture* cpu = loadKTX(filename, isNormalMap, isHeightMap);
    if (!cpu) return 0;

    GLuint tex = uploadKTX_OpenGL(cpu);

    ktxTexture_Destroy(cpu);
    return tex;
}

ktxTexture* engine::ktxLoader::loadKTX(const std::string& filename, bool isNormalMap, bool isHeightMap)
{
    ktxTexture* kTexture = nullptr;

    // Load KTX1 or KTX2
    if (ktxTexture_CreateFromNamedFile(
        filename.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture) != KTX_SUCCESS)
    {
        return nullptr;
    }

    // Detect KTX2 (correct for KTX 4.4.x)
    if (kTexture->classId == ktxTexture2_c)
    {
        ktxTexture2* kTexture2 = reinterpret_cast<ktxTexture2*>(kTexture);

        ktx_transcode_fmt_e fmt = chooseTranscodeFormat(isNormalMap, isHeightMap);

        if (ktxTexture2_TranscodeBasis(kTexture2, fmt, 0) != KTX_SUCCESS)
        {
            ktxTexture_Destroy(kTexture);
            return nullptr;
        }
    }

    // CPU-side KTX object ready
    return kTexture;
}

GLuint engine::ktxLoader::uploadKTX_OpenGL(ktxTexture* kTexture)
{
    if (!kTexture)
        return 0;

    GLuint tex = 0;
    glGenTextures(1, &tex);

    GLenum target = 0;
    GLenum glerror = 0;

    // Your version of libktx requires 4 arguments
    if (ktxTexture_GLUpload(kTexture, &tex, &target, &glerror) != KTX_SUCCESS)
    {
        glDeleteTextures(1, &tex);
        return 0;
    }

    // Apply sampler parameters
    glBindTexture(target, tex);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}


ktx_transcode_fmt_e engine::ktxLoader::chooseTranscodeFormat(bool isNormalMap, bool isHeightMap)
{
    if (isNormalMap) return KTX_TTF_BC5_RG;
    if (isHeightMap) return KTX_TTF_BC4_R;
    return KTX_TTF_BC7_RGBA;
}


int engine::ktxLoader::getKTXComponents(ktxTexture* tex, bool isNormalMap, bool isHeightMap)
{
    if (isNormalMap) return 2;   // BC5_RG
    if (isHeightMap) return 1;   // BC4_R
    return 4;                    // BC7_RGBA
}

