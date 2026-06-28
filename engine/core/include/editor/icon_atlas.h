#pragma once

#include "imgui.h"

#include <unordered_map>
#include <string>

namespace engine
{
    struct IconUV
    {
        ImVec2 uv0;
        ImVec2 uv1;
    };

    class IconAtlas
    {
    public:
        IconAtlas() = default;

        bool load(const std::string& filePath,
            int iconSize,
            int atlasWidth,
            int atlasHeight);

        bool isAtlasLoaded() { return m_textureID > 0; }

        void generateUVs(std::vector<unsigned int> icons);

        unsigned int getTextureID() const { return m_textureID; }

        const IconUV& getUV(unsigned int icon) const;

    private:
        unsigned int m_textureID{};

        int m_iconSize{};
        int m_atlasWidth{};
        int m_atlasHeight{};

        std::unordered_map<unsigned int, IconUV> m_uvs;
    };
}