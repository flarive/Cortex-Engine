#pragma once

#include "../imgui_element.h"

#include "../../materials/material.h"

#include "../../common_defines.h"

#include "../../../include/managers/texture_manager.h"

#include "../../../include/editor/editor_helper.h"

namespace engine
{
    class MaterialWidget final : public ImGuiElement
    {
    public:
        MaterialWidget();
        ~MaterialWidget();

        void init() override;

        void setMaterials(std::vector<std::shared_ptr<Material>>& materials);

    protected:
        void draw() override;

    private:
        
        const float ROW_HEIGHT = 12.0f;
        const ImVec4 GREY_TEXT_COLOR = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        bool m_isHeaderExpanded{ false };

        std::vector<std::weak_ptr<Material>> m_materials{};

        void displayMaterial(const std::shared_ptr<Material>& material, size_t index);

        void displayColor(const Color& color, const std::string& textType);
        void displayTexture(const TextureData* textData, const std::string& textType);

        engine::TagColors getImageSizeTagColor(int width, int height);
    };
}