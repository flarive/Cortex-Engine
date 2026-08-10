#pragma once

#include "../imgui_element.h"

#include "../../models/mesh.h"

namespace engine
{
    class MeshWidget final : public ImGuiElement
    {
    public:
        MeshWidget() : ImGuiElement(Category::Widget, "MeshSubComponentWidget") {}

        void init() override;

        void setMeshes(const std::vector<std::shared_ptr<Mesh>>& meshes);

    protected:
        void draw() override;

    private:

        bool m_isHeaderExpanded{ true };

        std::vector<std::weak_ptr<Mesh>> m_meshes{};
    };
}