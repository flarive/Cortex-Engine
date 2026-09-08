#pragma once

#include "../imgui_element.h"

#include "../../models/mesh.h"
#include "../../models/skeleton.h"

namespace engine
{
    class SkeletonWidget final : public ImGuiElement
    {
    public:
        SkeletonWidget();
        ~SkeletonWidget();

        void init() override;

        void setSkeleton(std::shared_ptr<Skeleton> skeleton);


    protected:
        void draw() override;

    private:

        bool m_isHeaderExpanded{ false };
        std::weak_ptr<Skeleton> m_skeleton{};

        void displaySkeleton(const std::shared_ptr<Skeleton>& skeleton);
    };
}