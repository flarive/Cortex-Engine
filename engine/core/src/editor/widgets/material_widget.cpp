#include "../../../include/editor/widgets/material_widget.h"


#include "../../../include/editor/editor_helper.h"


void engine::MaterialWidget::init()
{
}

void engine::MaterialWidget::draw()
{
    //ImGui::ColorEdit3("Tint", m_color);


    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingHeader("Material", ImGuiTreeNodeFlags_None))
    {
        
    }
}