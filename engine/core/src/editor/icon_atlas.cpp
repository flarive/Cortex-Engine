#include "../../include/editor/icon_atlas.h"
#include "../../include/texture.h"

bool engine::IconAtlas::load(const std::string& filePath,
    int iconSize,
    int atlasWidth,
    int atlasHeight)
{
    m_iconSize = iconSize;
    m_atlasWidth = atlasWidth;
    m_atlasHeight = atlasHeight;

    m_textureID = Texture::loadGLTextureFromFile(
        filePath.c_str(),
        "icons",
        true, true, false
    );

    if (m_textureID == 0)
        return false;

    //generateUVs();
    return true;
}

void engine::IconAtlas::generateUVs(std::vector<unsigned int> iconsEnum)
{
    const int iconsPerRow = m_atlasWidth / m_iconSize;

    for (int i = 0; i < iconsEnum.size(); i++)
    {
        unsigned int icon = iconsEnum[i];

        int x = (i % iconsPerRow) * m_iconSize;
        int y = (i / iconsPerRow) * m_iconSize;

        IconUV uv;
        uv.uv0 = ImVec2(
            float(x) / m_atlasWidth,
            float(y) / m_atlasHeight
        );
        uv.uv1 = ImVec2(
            float(x + m_iconSize) / m_atlasWidth,
            float(y + m_iconSize) / m_atlasHeight
        );

        m_uvs[icon] = uv;
    }
}

const engine::IconUV& engine::IconAtlas::getUV(unsigned int icon) const
{
    return m_uvs.at(icon);
}