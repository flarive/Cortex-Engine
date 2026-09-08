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

void engine::SkeletonWidget::setSkeleton(std::shared_ptr<engine::Skeleton> skeleton)
{
    m_skeleton = skeleton;
}

void engine::SkeletonWidget::draw()
{
    ImGui::PushFont(ImGui::Spectrum::fontSmall2);

    std::string header = std::format("Skeleton ({})", m_skeleton.expired() ? 0 : 1);

    ImGui::SetNextItemOpen(m_isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader(header.c_str(), ImGuiTreeNodeFlags_None, EditorHelper::im_grey_dark))
    {
        // Lock the weak_ptr to get a shared_ptr
        if (auto sharedSkeleton = m_skeleton.lock())
        {
            displaySkeleton(sharedSkeleton);
        }
    }

    ImGui::PopFont();
}

void engine::SkeletonWidget::displaySkeleton(const std::shared_ptr<Skeleton>& skeleton)
{
    const std::string tableUniqueID = "SkeletonBonesTable";
    if (ImGui::BeginTable(tableUniqueID.c_str(), 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthFixed, 100);

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Nbr bones");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("%u bones", skeleton->getBoneCount());
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }

    displaySkeletonBones(skeleton->getBones());
}



void engine::SkeletonWidget::displaySkeletonBones(const std::vector<SkeletonBone>& bones)
{
    // Draw roots
    for (size_t i = 0; i < bones.size(); ++i)
    {
        if (bones[i].parentIndex == -1)
            drawBoneRecursive(bones, static_cast<int>(i));
    }
}

void engine::SkeletonWidget::drawBoneRecursive(const std::vector<SkeletonBone>& bones, int index)
{
    const SkeletonBone& bone = bones[index];

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    bool open = ImGui::TreeNodeEx(bone.name.c_str(), flags);

    if (open)
    {
        // Draw children
        for (size_t i = 0; i < bones.size(); ++i)
        {
            if (bones[i].parentIndex == index)
                drawBoneRecursive(bones, static_cast<int>(i));
        }

        ImGui::TreePop();
    }
}

engine::SkeletonWidget::~SkeletonWidget()
{
    logger.trace("SkeletonWidget desctructor called");
}