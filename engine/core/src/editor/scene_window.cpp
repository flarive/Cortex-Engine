#include "../../include/editor/scene_window.h"

#include "../../include/editor/editor_helper.h"

void engine::SceneWindow::renderHierarchyWidget()
{
    if (m_rootEntity)
    {
        ImGui::BeginChild("EntityTreeRegion", ImVec2(0, 0), true);
        displayEntityHierarchy(m_rootEntity);
        ImGui::EndChild();
    }
}

void engine::SceneWindow::displayEntityHierarchy(const std::shared_ptr<Entity>& entity)
{
    bool isSelected = (m_selectedEntity == entity);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DrawLinesFull;
    if (entity->children.empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    auto entityType = entity->getType();

    if (isSelected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
        ImGui::PushStyleColor(ImGuiCol_Text, EditorHelper::getEntityColor(entityType));
    }

    ImGui::PushID(entity.get()); // unique ID per entity

    GLuint tex = EditorHelper::getIconAtlasTexture();
    IM_ASSERT(tex != 0);

    // Draw the icon
    auto uvEntityType = EditorHelper::getEntityTypeSmallIcon(EditorHelper::convertEntityTypeToAtlasIcon(entityType, 16));
    ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(16, 16), uvEntityType.uv0, uvEntityType.uv1);

    ImGui::SameLine();

    // Tree node
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(11.f, 0.f));
    bool nodeOpen = ImGui::TreeNodeEx("##tree", flags, "%s", entity->name.c_str());
    ImGui::PopStyleVar(1);

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        m_selectedEntity = entity;

        if (m_onSelectionChanged)
            m_onSelectionChanged(m_selectedEntity);
    }

    // Image button at the end of the line
    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 28.0f); // align to right side

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(-2.f, 0.f));

    // Draw the show/hide entity button
    auto uvShowHide = EditorHelper::getIcon(entity->enabled ? EditorIcon::show : EditorIcon::hide);
    if (ImGui::ImageButton("##visible", (ImTextureID)(intptr_t)tex, ImVec2(16, 16), uvShowHide.uv0, uvShowHide.uv1))
    {
        entity->setEnabled(!entity->enabled);
    }

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);



    if (isSelected)
        ImGui::PopStyleColor();

    // Draw children
    if (nodeOpen)
    {
        for (const auto& child : entity->children)
            displayEntityHierarchy(child);
        ImGui::TreePop();
    }

    ImGui::PopID();
}