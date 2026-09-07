#include "../../../include/editor/widgets/skeleton_widget.h"


#include "../../../include/editor/editor_helper.h"

#include "../../../include/managers/log_manager.h"

#include <string>
#include <format>


engine::SkeletonWidget::SkeletonWidget() : ImGuiElement(Category::Widget, "SkeletonSubComponentWidget")
{
    logger.trace("SkeletonWidget constructor called");
}

void engine::SkeletonWidget::init()
{

}

void engine::SkeletonWidget::draw()
{
    ImGui::PushFont(ImGui::Spectrum::fontSmall2);

    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader("Skeleton", ImGuiTreeNodeFlags_None, EditorHelper::im_grey_dark))
    {
        
    }

    ImGui::PopFont();
}

engine::SkeletonWidget::~SkeletonWidget()
{
    logger.trace("SkeletonWidget desctructor called");
}