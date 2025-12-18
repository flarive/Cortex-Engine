#include "../../include/editor/editor_helper.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "extensions/toggle/imgui_toggle.h"
#include "extensions/toggle/imgui_toggle_palette.h"

#include <imgui_internal.h>

void engine::EditorHelper::renderVectorTable(const std::vector<std::string>& items, const EditorProperty& property)
{
    static unsigned short prev_selected_row = -1;
    static unsigned short selected_row = -1;

    if (ImGui::BeginTable("MyTable", 1, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Column", ImGuiTableColumnFlags_None);


        // Target compact row height
        const float text_h = ImGui::GetTextLineHeight();       // tighter than GetTextLineHeightWithSpacing()
        const float row_height = text_h + 2.0f;                    // add a couple of pixels if needed

        unsigned short row_index = 0;
        for (const auto& value : items)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            // Compute full row rect spanning the table inner rect
            ImVec2 cursor = ImGui::GetCursorScreenPos();
            ImGuiTable* table = ImGui::GetCurrentTable();

            ImVec2 min(table->InnerRect.Min.x, cursor.y);
            ImVec2 max(table->InnerRect.Max.x, cursor.y + row_height);

            // Hit-test over the full row to support hover/active/click
            ImGui::PushID(row_index);
            ImGui::InvisibleButton("##row_hit", ImVec2(max.x - min.x, row_height));
            bool hovered = ImGui::IsItemHovered();
            bool held = ImGui::IsItemActive();             // while mouse down on this row
            bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
            if (clicked)
                selected_row = row_index;
            ImGui::PopID();

            const bool is_selected = (row_index == selected_row);
            if (is_selected && prev_selected_row != selected_row)
            {
                property.function(selected_row);
                prev_selected_row = selected_row;
            }

            // Color policy
            ImU32 col = IM_COL32(50, 50, 50, 255);
            if (is_selected) col = IM_COL32(70, 70, 70, 255);
            else if (hovered) col = IM_COL32(70, 70, 70, 255);


            ImGui::TablePushBackgroundChannel();

            // Draw background behind content
            ImDrawList* dl = ImGui::GetWindowDrawList();
            dl->AddRectFilled(min, max, col, ROUNDING);

            // Draw border (use a contrasting color, e.g., white or gray)
            ImU32 border_col = ImGui::GetColorU32(ImGuiCol_Border);
            dl->AddRect(min, max, border_col, ROUNDING, 0, 1.0f);

            ImGui::TablePopBackgroundChannel();

            // Add horizontal padding
            ImGui::SetCursorScreenPos(ImVec2(cursor.x + 5.0f, cursor.y));

            // Draw content
            ImGui::Text("%s", value.c_str());

            ++row_index;
        }

        ImGui::EndTable();
    }
}
