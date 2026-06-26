#pragma once

#include "imgui.h"
#include "../ecs/entity.h"

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

        GLuint getTextureID() const { return m_textureID; }

        const IconUV& getUV(EntityType type) const;

    private:
        void generateUVs();

    private:
        GLuint m_textureID{};

        int m_iconSize{};
        int m_atlasWidth{};
        int m_atlasHeight{};

        std::unordered_map<EntityType, IconUV> m_uvs;
    };
}