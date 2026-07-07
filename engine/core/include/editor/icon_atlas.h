#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

#include <glad/glad.h>
#include <imgui.h>

namespace engine {

    struct IconUV final {
        ImVec2 uv0; // 0.0 - 0.1
        ImVec2 uv1; // 0.0 - 0.1
    };

    struct IconInfo final {
        int x;      // pixel position in atlas
        int y;
        int w;      // icon width
        int h;      // icon height
    };

    class IconAtlas final
    {
    public:
        bool load(const std::string& filePath,
            int atlasWidth,
            int atlasHeight);

        void registerIcon(unsigned int iconEnum,
            int x, int y,
            int w, int h);

        void generateUVs();

        const IconUV& getUV(unsigned int iconEnum) const;

        GLuint getTextureID() const { return m_textureID; }
        bool isAtlasLoaded() const { return m_textureID != 0; }

    private:
        GLuint m_textureID = 0;

        int m_atlasWidth = 0;
        int m_atlasHeight = 0;

        std::unordered_map<unsigned int, IconInfo> m_iconInfos;
        std::unordered_map<unsigned int, IconUV> m_uvs;
    };

} // namespace engine
