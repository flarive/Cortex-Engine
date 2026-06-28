#include "../../include/editor/editor_helper.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "extensions/toggle/imgui_toggle.h"
#include "extensions/toggle/imgui_toggle_palette.h"

#include "../../include/managers/entity_manager.h"

#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <format>


std::unordered_map<std::string, GLuint> engine::EditorHelper::m_iconTextureCache; // Define the static member
std::unordered_map<std::string, bool> engine::EditorHelper::m_iconToggleStates; // Define the static member
std::unordered_map<std::string, GLuint> engine::EditorHelper::m_iconActionTextureCache; // Define the static member
std::unordered_map<engine::EntityType, GLuint> engine::EditorHelper::m_iconSmallTextureCache; // Define the static member
engine::IconAtlas engine::EditorHelper::m_iconAtlas; // Define the static member


void engine::EditorHelper::renderDynamicProperties(std::shared_ptr<Component> component, const std::string& componentType)
{
    if (!component)
        return;

    // draw component properties dynamically
    auto properties = component->getPublicProperties();
    auto componentName = component->getName();

    if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, ITEM_LABEL_WIDTH);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch);
        properties.forEach([&](const std::string& key, EditorProperty& property)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                if (property.type & label)
                {
					// 1 column label full width
                    ImGui::SetNextItemWidth(-FLT_MIN);
                    ImGui::Text(property.display.c_str());
                }
                else
                {
                    if (property.type & noheader)
                    {
                        // Full width for readonly property: Span both columns
                        float fullWidth = ImGui::GetContentRegionAvail().x;
                        ImGui::SetNextItemWidth(fullWidth);
                    }
                    else if (property.type & editable | readonly)
                    {
                        // 2 columns, header and property
                        ImGui::Text(property.display.c_str());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetNextItemWidth(80);
                    }
 
                    if (float* pValue = std::get_if<float>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%.2f%s", *pValue, !property.suffix.empty() ? std::format(" {}", property.suffix).c_str() : "");
                        }
                        else {
                            if (ImGui::DragFloat(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue, property.step, property.min, property.max, property.format.c_str(), ImGuiSliderFlags_NoRoundToFormat))
                            {
                                // float value changed
                                component->setProperty(key, *pValue);
                            }
                        }
                    }
                    else if (int* pValue = std::get_if<int>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%i%s", *pValue, !property.suffix.empty() ? std::format(" {}", property.suffix).c_str() : "");
                        }
                        else {
                            if (ImGui::DragInt(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue, property.step, static_cast<int>(property.min), static_cast<int>(property.max), property.format.c_str(), ImGuiSliderFlags_NoRoundToFormat))
                            {
                                // int value changed
                                component->setProperty(key, *pValue);
                            }
                        }
                    }
                    else if (unsigned int* pValue = std::get_if<unsigned int>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%u%s", *pValue, !property.suffix.empty() ? std::format(" {}", property.suffix).c_str() : "");
                        }
                        else {
                            if (ImGui::DragScalar(std::format("##{}{}{}", componentName, componentType, key).c_str(), ImGuiDataType_U32, pValue, property.step))
                            {
                                // unsigned int value changed
                                component->setProperty(key, *pValue);
                            }
                        }
                    }
                    else if (bool* pValue = std::get_if<bool>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%s", (*pValue == 1 ? "Yes" : "No"));
                        }
                        else {
                            if (ImGui::Checkbox(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue))
                            {
                                // bool value changed
                                component->setProperty(key, *pValue);
                            }
                        }
                    }
                    else if (std::string* pValue = std::get_if<std::string>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%s", pValue->c_str());
                        }
                        else {
                            // Create a temporary buffer for ImGui::InputText
                            char buffer[256];
                            strncpy(buffer, pValue->c_str(), sizeof(buffer));
                            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination

                            if (ImGui::InputText(std::format("##{}{}{}", componentName, componentType, key).c_str(), buffer, sizeof(buffer)))
                            {
                                // string value changed
                                *pValue = buffer;
                                component->setProperty(key, *pValue);
                            }
                        }
                    }
                    else if (glm::vec2* pValue = std::get_if<glm::vec2>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%i", *pValue);
                        }
                        else
                        {
                            if (ImGui::BeginTable("MyTable", 2, ImGuiTableFlags_SizingStretchSame))
                            {
                                ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                                ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 75.0f);

                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                if (drawCustomDragFloat("X", std::format("##{}{}{}X", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &pValue->x, property.step)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::TableSetColumnIndex(1);
                                if (drawCustomDragFloat("Y", std::format("##{}{}{}Y", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &pValue->y, property.step)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::EndTable();
                            }
                        }
                    }
                    else if (glm::vec3* pValue = std::get_if<glm::vec3>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("%i", *pValue);
                        }
                        else
                        {
                            if (ImGui::BeginTable("MyTable", 3, ImGuiTableFlags_SizingStretchSame))
                            {
                                ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                                ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 75.0f);
                                ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 75.0f);

                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                if (drawCustomDragFloat("X", std::format("##{}{}{}X", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &pValue->x, property.step)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::TableSetColumnIndex(1);
                                if (drawCustomDragFloat("Y", std::format("##{}{}{}Y", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &pValue->y, property.step)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::TableSetColumnIndex(2);
                                if (drawCustomDragFloat("Z", std::format("##{}{}{}Z", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, &pValue->z, property.step)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::EndTable();
                            }
                        }
                    }
                    else if (glm::vec4* pValue = std::get_if<glm::vec4>(&property.value))
                    {
                        if (property.type & readonly) {
                            ImGui::Text("(%f, %f, %f, %f)", pValue->x, pValue->y, pValue->z, pValue->w);
                        }
                        else
                        {
                            ImGui::PushItemWidth(200.0f);
                            if (ImGui::ColorEdit3(std::format("##{}{}{}", componentName, componentType, key).c_str(), glm::value_ptr(*pValue), ImGuiColorEditFlags_NoLabel)) {
                                component->setProperty(key, *pValue);
                            }
                            ImGui::PopItemWidth();
                        }
                    }
                    else if (std::vector<std::string>* pValue = std::get_if<std::vector<std::string>>(&property.value))
                    {
                        if (property.type & combobox) {
                            // combobox list
                            EditorHelper::renderStringVectorComboboxTable(key, *pValue, property);
                        }
                        else if (property.type & readonly) {
                            // button list
                            EditorHelper::renderStringVectorButtonListTable(key , *pValue, property);
                        }
                        else
                        {
                            if (ImGui::BeginTable("MyTable", 1, ImGuiTableFlags_SizingStretchSame))
                            {
                                ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_None);

                                for (std::string value : *pValue)
                                {
                                    ImGui::TableNextRow();
                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::Text("%s", value.c_str());
                                }

                                ImGui::EndTable();
                            }
                        }
                    }
                }
        });
        ImGui::EndTable();
    }
}

void engine::EditorHelper::drawCustomLabel(const char* text, const ImVec2& position, const ImVec2& size, float rounding, ImU32 backgroundColor, ImU32 foregroundColor)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 min = position;
    ImVec2 max = ImVec2(position.x + size.x, position.y + size.y);

    // Draw red rectangle with rounded left corners
    draw_list->AddRectFilled(min, max, backgroundColor, rounding, ImDrawFlags_RoundCornersLeft);

    // Draw text centered
    ImVec2 text_size = ImGui::CalcTextSize(text);
    ImVec2 text_pos = ImVec2(
        position.x + (size.x - text_size.x) * 0.5f,
        position.y + (size.y - text_size.y) * 0.5f
    );
    draw_list->AddText(text_pos, foregroundColor, text);
}

bool engine::EditorHelper::drawCustomDragFloat(const char* text, const char* name, const ImVec2& position, const ImVec2& size, float rounding, float width, ImU32 backgroundColor, ImU32 foregroundColor, float* value, float step)
{
    drawCustomLabel(text, position, size, rounding, backgroundColor, foregroundColor);

    // Move cursor to the end of the label manually
    ImGui::SetCursorScreenPos(ImVec2(position.x + size.x, position.y));

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size2 = ImVec2(width, ImGui::GetFrameHeight()); // Width can be adjusted

    // Draw background with rounded right corners
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(pos, ImVec2(pos.x + size2.x, pos.y + size2.y),
        IM_COL32(50, 50, 50, 255), rounding,
        ImDrawFlags_RoundCornersRight);

    // Render the DragFloat widget
    ImGui::SetCursorScreenPos(pos); // Reset cursor to draw over the background
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // Transparent background
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
    ImGui::SetNextItemWidth(width);
    bool res = ImGui::DragFloat(name, value, step, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None, ImDrawFlags_RoundCornersRight);
    ImGui::PopStyleColor(3);

    return res;
}

void engine::EditorHelper::renderSliderIntWithLabel(const char* label, const char* key, int& value, int& lastValue, int min, int max, std::function<void(std::string, SceneSetting)> sceneSettingChanged)
{
    static bool isDraggingSlider = false;

    ImGui::SetNextItemWidth(FIELD_WIDTH);

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::SliderInt(
        label,
        &value,
        min,  // Minimum value
        max   // Maximum value
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (sceneSettingChanged && lastValue != value)
        {
            sceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::EditorHelper::renderSliderFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, const char* format, std::function<void(std::string, SceneSetting)> sceneSettingChanged)
{
    static bool isDraggingSlider = false;

    ImGui::SetNextItemWidth(FIELD_WIDTH);

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::SliderFloat(
        label,
        &value,
        min,  // Minimum value
        max,   // Maximum value
        format // Display format
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (sceneSettingChanged && lastValue != value)
        {
            sceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::EditorHelper::renderDragFloatWithLabel(const char* label, const char* key, float& value, float& lastValue, float min, float max, float step, const char* format, std::function<void(std::string, SceneSetting)> sceneSettingChanged)
{
    static bool isDraggingSlider = false;

    ImGui::SetNextItemWidth(FIELD_WIDTH);

    // Use DragInt with a step of 256 (or your desired step)
    ImGui::DragFloat(
        label,
        &value,
        step, // Step size (1.0f means it increments by 1 per "tick", but you can use 256.0f for 256 steps)
        min,  // Minimum value
        max,   // Maximum value
        format // Display format
    );
    isDraggingSlider = ImGui::IsItemActive();

    // Apply changes only on release
    if (!isDraggingSlider && ImGui::IsItemDeactivatedAfterEdit())
    {
        if (sceneSettingChanged && lastValue != value)
        {
            sceneSettingChanged(key, value);
            lastValue = value;
        }
    }
}

void engine::EditorHelper::renderStringVectorButtonListTable(const std::string& key, const std::vector<std::string>& items, const EditorProperty& property)
{
    static unsigned short prev_selected_row = -1;
    static unsigned short selected_row = -1;

    if (ImGui::BeginTable(std::format("Table{}", key).c_str(), 1, ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Column", ImGuiTableColumnFlags_WidthStretch);

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

void engine::EditorHelper::renderStringVectorComboboxTable(
    const std::string& key,
    const std::vector<std::string>& items,
    const EditorProperty& property)
{
    // One selected index per combobox key
    static std::unordered_map<std::string, int> selectedIndex;
    int& currentIdx = selectedIndex[key];

    // Fix index if out of range
    if (items.empty())
    {
        currentIdx = -1;
        return;
    }
    if (currentIdx < 0 || currentIdx >= (int)items.size())
        currentIdx = 0;

    const std::string comboId = "##" + key;
    const char* preview = items[currentIdx].c_str();

    ImGui::SetNextItemWidth(200.0f);   // width in pixels
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 4)); // X, Y padding
    if (ImGui::BeginCombo(comboId.c_str(), preview))
    {
        for (unsigned int i = 0; i < (int)items.size(); ++i)
        {
            bool isSelected = (i == currentIdx);

            if (ImGui::Selectable(items[i].c_str(), isSelected))
            {
                currentIdx = i;
                property.function(i);
            }

            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleVar();
}

void engine::EditorHelper::addToolbarIconButton(const std::string& icon, std::function<void()> onClick)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));

    // Set default or toggled colors BEFORE rendering the button
    if (m_iconToggleStates[icon]) {
        // Toggled ON colors
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE400));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE700));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE500));
    }
    else {
        // Toggled OFF colors
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
    }

    GLuint my_texture_id = getIconTexture(icon, "editor", "icons");
    if (ImGui::ImageButton(std::format("##{}", icon).c_str(), (ImTextureID)(intptr_t)my_texture_id, ImVec2(18, 18)))
    {
        resetIconToggleStates(); // Turn all off
        setIconToggleState(icon, true); // Turn only this one on
        if (onClick) onClick(); // Call the provided function
    }

    if (ImGui::IsItemHovered()) {
        // Optional: Additional hover effects
    }

    ImGui::PopStyleVar(3); // Pop rounding, border, padding
    ImGui::PopStyleColor(3); // Pop colors
}

void engine::EditorHelper::addDiscreetIconButton(bool& state, const std::string& icon_off, const std::string& icon_on, std::function<void()> onClick)
{
    GLuint buttonIcon = state ? getIcon(icon_on) : getIcon(icon_off);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 2.f));

    if (ImGui::ImageButton(std::format("##{}", icon_off).c_str(), (ImTextureID)(intptr_t)buttonIcon, ImVec2(16, 16)))
    {
        if (onClick)
            onClick();

        state = !state;
    }

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(4);
}

GLuint engine::EditorHelper::getIconTexture(const std::string& key, const std::string& prefix, const std::string& folder)
{
    auto it = m_iconTextureCache.find(key);
    if (it != m_iconTextureCache.end())
    {
        return it->second;
    }
    else
    {
        auto iconName = std::format("{}_{}.png", prefix, key);
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), folder);

        m_iconTextureCache.insert(std::make_pair(key, iconTexture));

        return iconTexture;
    }
}

void engine::EditorHelper::resetIconToggleStates()
{
    for (auto& [k, v] : m_iconToggleStates) v = false; // Turn all off
}

void engine::EditorHelper::setIconToggleState(const std::string& key, bool state)
{
    m_iconToggleStates[key] = state;
}

// Returns true if the toolbar was started successfully.
// Outputs the background rect in screen space if outBgMin/outBgMax are provided.
// `outStartCursorLocal` is where your first item will be placed (window-local).
bool engine::EditorHelper::BeginCenteredToolbar(
    int   iconCount,
    float iconSize,
    float iconSpacing,              // <0 => use style.ItemSpacing.x
    float bgPaddingX,
    float bgPaddingY,
    float bgRounding,
    ImU32 bgColor,
    ImVec2* outBgMin,
    ImVec2* outBgMax,
    ImVec2* outStartCursorLocal
)
{
    if (iconCount <= 0 || iconSize <= 0.0f)
        return false;

    if (iconSpacing < 0.0f)
        iconSpacing = ImGui::GetStyle().ItemSpacing.x;

    // Compute content widths
    const float groupWidth = iconCount * iconSize + (iconCount - 1) * iconSpacing;
    const float bgWidth = groupWidth + bgPaddingX * 2.0f;
    const float bgHeight = iconSize + bgPaddingY * 2.0f;

    // Available width in current region/line
    const float avail = ImGui::GetContentRegionAvail().x;

    // Horizontal offset to center the BACKGROUND (and thus the group)
    float offsetX = 0.0f;
    if (bgWidth < avail)
        offsetX = (avail - bgWidth) * 0.5f;

    // Anchor positions in WINDOW-LOCAL space
    ImVec2 cursorStart = ImGui::GetCursorPos(); // window-local
    ImVec2 bgMin = ImVec2(cursorStart.x + offsetX, cursorStart.y);
    ImVec2 bgMax = ImVec2(bgMin.x + bgWidth, bgMin.y + bgHeight);

    // Convert to SCREEN space for draw list
    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 bgMinScreen = ImVec2(winPos.x + bgMin.x, winPos.y + bgMin.y);
    ImVec2 bgMaxScreen = ImVec2(winPos.x + bgMax.x, winPos.y + bgMax.y);

    // Draw background (behind)
    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(bgMinScreen, bgMaxScreen, bgColor, bgRounding);

    // Move cursor inside the background for the group
    ImVec2 groupStart = ImVec2(bgMin.x + bgPaddingX, bgMin.y + bgPaddingY);
    ImGui::SetCursorPos(groupStart);

    // Start a group so the caller's items are kept together
    ImGui::BeginGroup();

    // Optional outputs
    if (outBgMin)              *outBgMin = bgMinScreen;
    if (outBgMax)              *outBgMax = bgMaxScreen;
    if (outStartCursorLocal)   *outStartCursorLocal = groupStart;

    return true;
}

void engine::EditorHelper::EndCenteredToolbar()
{
    ImGui::EndGroup();
    // If you want to ensure layout continues on a new line below the background, you could add:
    // ImGui::Dummy(ImVec2(0.0f, 0.0f)); // not strictly necessary
}

GLuint engine::EditorHelper::getIcon(const std::string& key)
{
    auto it = m_iconActionTextureCache.find(key);
    if (it != m_iconActionTextureCache.end())
    {
        return it->second;
    }
    else {
        auto iconName = std::format("icon_{}_16x16.png", key);
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons", false, true, false);

        m_iconActionTextureCache.insert(std::make_pair(key, iconTexture));

        return iconTexture;
    }
}
GLuint engine::EditorHelper::getEntityTypeSmallIcon(const engine::EntityType entityType)
{
    auto it = m_iconSmallTextureCache.find(entityType);
    if (it != m_iconSmallTextureCache.end())
    {
        return it->second;
    }
    else
    {
        auto iconName = std::format("icon_{}_16x16.png", static_cast<int>(entityType));
        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons", false);

        m_iconSmallTextureCache.insert(std::make_pair(entityType, iconTexture));

        return iconTexture;
    }
}

const engine::IconUV& engine::EditorHelper::getEntityTypeMediumIcon(const engine::EditorIcon icon)
{
    if (!m_iconAtlas.isAtlasLoaded())
    {
        m_iconAtlas.load(
            "editor_atlas_48x48.png",
            48,     // icon size
            480,    // atlas width
            480     // atlas height
        );

		std::vector<unsigned int> icons = {
            static_cast<unsigned int>(EditorIcon::undefined),
            static_cast<unsigned int>(EditorIcon::entity_model),
			static_cast<unsigned int>(EditorIcon::entity_primitive),
			static_cast<unsigned int>(EditorIcon::entity_light),
			static_cast<unsigned int>(EditorIcon::entity_camera),
			static_cast<unsigned int>(EditorIcon::entity_particleSystem),
			static_cast<unsigned int>(EditorIcon::entity_terrain)
		};

        m_iconAtlas.generateUVs(icons);
    }

    return m_iconAtlas.getUV(static_cast<unsigned int>(icon));
}

GLuint engine::EditorHelper::getIconAtlasTexture()
{
    return m_iconAtlas.getTextureID();
}

/// <summary>
/// Hack FL !!!!!!!!!!!!!!! Collapsing header with checkbox
/// </summary>
/// <param name="label"></param>
/// <param name="p_checked"></param>
/// <param name="flags"></param>
/// <returns></returns>
bool engine::EditorHelper::collapsingCheckboxHeader(const char* label, bool* p_checked, ImGuiTreeNodeFlags flags, std::function<void(bool)> onCheck)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    char result[100];
    snprintf(result, sizeof(result), "%s%s", "##collapsingHeaderCheckbox", label);

    ImGuiID id = window->GetID(result);
    flags |= ImGuiTreeNodeFlags_CollapsingHeader;
    //if (p_visible)
    flags |= ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_ClipLabelForTrailingButton;

    bool is_open = ImGui::TreeNodeBehavior(id, flags, result);

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeight());
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    float checkbox_width = ImGui::GetFrameHeight();
    //ImVec2 checkbox_pos(bb.Max.x - checkbox_width - style.ItemSpacing.x, pos.y - 42);
    ImVec2 checkbox_pos(32, pos.y - 72);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // Reduce padding
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);      // Reduce border size
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);       // Reduce corner rounding

    ImGui::SetCursorPos(checkbox_pos);

    if (ImGui::Checkbox(label, p_checked))
    {
        onCheck(*p_checked);
    }

    ImGui::PopStyleVar(3); // Restore the previous style

    return is_open;
}

