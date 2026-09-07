#pragma once

#include "../imgui_element.h"

#include "../../models/mesh.h"

namespace engine
{
    class SkeletonWidget final : public ImGuiElement
    {
    public:
        SkeletonWidget();
        ~SkeletonWidget();

        void init() override;

        //void setSkeleton(const std::vector<std::shared_ptr<Mesh>>& meshes);

    protected:
        void draw() override;

    private:

        bool m_isHeaderExpanded{ false };

        //std::vector<std::weak_ptr<Mesh>> m_meshes{};

        //void displayMesh(const std::shared_ptr<engine::Mesh>& mesh);
    };
}