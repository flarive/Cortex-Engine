#include "../../include/editor/icon_atlas.h"
#include "../../include/texture.h"

bool engine::IconAtlas::load(const std::string& filePath, int atlasWidth, int atlasHeight)
{
    m_atlasWidth = atlasWidth;
    m_atlasHeight = atlasHeight;

    m_textureID = Texture::loadTexture(filePath.c_str()); // TextureFlag_InvertY

    return (m_textureID != 0);
}

void engine::IconAtlas::registerIcon(unsigned int iconEnum, int x, int y, int w, int h)
{
    IconInfo info;
    info.x = x;
    info.y = y;
    info.w = w;
    info.h = h;

    m_iconInfos[iconEnum] = info;
}

void engine::IconAtlas::generateUVs()
{
    m_uvs.clear();

    for (auto& kv : m_iconInfos)
    {
        unsigned int icon = kv.first;
        const IconInfo& info = kv.second;

        IconUV uv;
        uv.uv0 = ImVec2(
            float(info.x) / m_atlasWidth,
            float(info.y) / m_atlasHeight
        );
        uv.uv1 = ImVec2(
            float(info.x + info.w) / m_atlasWidth,
            float(info.y + info.h) / m_atlasHeight
        );

        m_uvs[icon] = uv;
    }
}

const engine::IconUV& engine::IconAtlas::getUV(unsigned int iconEnum) const
{
    return m_uvs.at(iconEnum);
}
