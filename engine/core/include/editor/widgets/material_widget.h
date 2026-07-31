#pragma once

#include "../imgui_element.h"

#include "../../materials/material.h"

#include "../../../include/managers/texture_manager.h"

namespace engine
{
    class MaterialWidget final : public ImGuiElement
    {
    public:
        MaterialWidget() : ImGuiElement(Category::Widget, "MaterialSubComponentWidget") {}

        void init() override;

        void setMaterial(std::shared_ptr<Material> material);

    protected:
        void draw() override;

    private:
        
        bool m_isHeaderExpanded{ true };

        std::weak_ptr<Material> m_material{};

        void displayTexture(const TextureData& textData);
    };
}