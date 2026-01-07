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

#include "extensions/imGuizmo/ImGuizmo.h"

#include <format>


std::unordered_map<std::string, GLuint> engine::EditorHelper::m_iconTextureCache; // Define the static member
std::unordered_map<std::string, bool> engine::EditorHelper::m_iconToggleStates; // Define the static member

bool engine::EditorHelper::m_displayViewTransformGuizmo{ true };
bool engine::EditorHelper::m_displayObjectTransformGuizmo{ false };


float engine::EditorHelper::viewWidth = 10.f; // for orthographic
const float engine::EditorHelper::camYAngle = 165.f / 180.f * 3.14159f;
const float engine::EditorHelper::camXAngle = 32.f / 180.f * 3.14159f;
float engine::EditorHelper::camDistance = 0.f;
int engine::EditorHelper::gizmoCount = 1;

//const float engine::EditorHelper::identityMatrix[16] =
//{ 1.f, 0.f, 0.f, 0.f,
//    0.f, 1.f, 0.f, 0.f,
//    0.f, 0.f, 1.f, 0.f,
//    0.f, 0.f, 0.f, 1.f };

bool engine::EditorHelper::firstFrame = true;
int engine::EditorHelper::lastUsing = 0;
//std::shared_ptr<engine::Entity> engine::EditorHelper::m_selectedEntity{};



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
                            if (ImGui::DragInt(std::format("##{}{}{}", componentName, componentType, key).c_str(), pValue, property.step, property.min, property.max, property.format.c_str(), ImGuiSliderFlags_NoRoundToFormat))
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
                                if (drawCustomDragFloat("X", std::format("##{}{}{}X", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, green, white, &pValue->x, 0.01f)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::TableSetColumnIndex(1);
                                if (drawCustomDragFloat("Y", std::format("##{}{}{}Y", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, red, white, &pValue->y, 0.01f)) {
                                    component->setProperty(key, *pValue);
                                }

                                ImGui::TableSetColumnIndex(2);
                                if (drawCustomDragFloat("Z", std::format("##{}{}{}Z", componentName, componentType, key).c_str(), ImGui::GetCursorScreenPos(), SIZE, ROUNDING, 50.0f, blue, white, &pValue->z, 0.01f)) {
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
                        if (property.type & readonly) {
                            EditorHelper::renderVectorTable(*pValue, property);
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
    bool res = ImGui::DragFloat(name, value, step, 0.0f, 0.0f, "%.2f");
    ImGui::PopStyleColor(3);

    return res;
}

void engine::EditorHelper::renderSliderIntWithLabel(const char* label, const char* key, int& value, int& lastValue, int min, int max, std::function<void(std::string, SceneSetting)> sceneSettingChanged)
{
    static bool isDraggingSlider = false;

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

void engine::EditorHelper::renderVectorTable(const std::vector<std::string>& items, const EditorProperty& property)
{
    static unsigned short prev_selected_row = -1;
    static unsigned short selected_row = -1;

    if (ImGui::BeginTable("MyTable", 1, ImGuiTableFlags_SizingStretchSame))
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

void engine::EditorHelper::addIconButton(const std::string& icon, std::function<void()> onClick)
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

void engine::EditorHelper::initRenderGuizmo(const std::shared_ptr<Camera> camera)
{
    camDistance = camera->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));
}

void engine::EditorHelper::resetIconToggleStates()
{
    for (auto& [k, v] : m_iconToggleStates) v = false; // Turn all off
}

void engine::EditorHelper::setIconToggleState(const std::string& key, bool state)
{
    m_iconToggleStates[key] = state;
}

void engine::EditorHelper::renderGuizmo(const std::shared_ptr<Entity> selectedEntity, const std::shared_ptr<Camera> camera, const float width, const float height, const bool fullscreen)
{
    if (!camera)
        return;

    glm::mat4 projection = camera->getProjectionMatrix(width, height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();

    // Convert glm::mat4 to const float*
    const float* projectionPtr = glm::value_ptr(projection);
    const float* viewPtr = glm::value_ptr(view);

    float* projectionPtr2 = glm::value_ptr(projection);
    float* viewPtr2 = glm::value_ptr(view);

    // Get the GLFW window position and size
    GLFWwindow* window = glfwGetCurrentContext();
    int windowX, windowY;
    glfwGetWindowPos(window, &windowX, &windowY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);


    if (m_displayObjectTransformGuizmo)
    {
        ImGuizmo::SetOrthographic(!camera->isPerspective);
        ImGuizmo::BeginFrame();

        ImGui::SetNextWindowPos(ImVec2(windowX + windowWidth / 2.0f - 128.0f, windowY + 10.0f));
        ImGui::SetNextWindowSize(ImVec2(256, 46));

        static bool open{};
        ImGui::Begin("Editor", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (selectedEntity && selectedEntity->name != EntityManager::ROOT_ENTITY_NAME)
        {
            glm::mat4& objectMatrix = selectedEntity->getWorldTransform();
            float* objectMatrixPtr = glm::value_ptr(objectMatrix);

            for (int matId = 0; matId < gizmoCount; matId++)
            {
                ImGuizmo::SetID(matId);

                editTransform(viewPtr, projectionPtr2, glm::value_ptr(objectMatrix[matId]), lastUsing == matId, selectedEntity, windowX, windowY, windowWidth, windowHeight);
                if (ImGuizmo::IsUsing())
                {
                    lastUsing = matId;
                }
            }
        }

        ImGui::End();
    }

    if (m_displayViewTransformGuizmo)
    {
        // Calculate the guizmo position relative to the window's top-right corner
        ImVec2 pos = !fullscreen ? ImVec2(windowX + windowWidth - 128.0f, windowY + 0.0f) : ImVec2(windowWidth - 128.0f, 0.0f);
        ImVec2 size = ImVec2(128, 128);

        // box displayed in the upper right corner
        if (ImGuizmo::ViewManipulate(viewPtr2, camDistance, pos, size, 0x10101010))
        {
            // Get the updated view matrix
            glm::mat4 updatedViewMatrix = glm::make_mat4(viewPtr2);

            // Decompose the original view matrix to get its rotation and position
            glm::vec3 originalPosition, newPosition, scale;
            glm::quat originalRotation;

            // Decompose the original view matrix
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(view, scale, originalRotation, originalPosition, skew, perspective);

            // Decompose the updated view matrix to get the new position
            glm::decompose(updatedViewMatrix, scale, originalRotation, newPosition, skew, perspective);

            // Reconstruct the view matrix with the new position and the original rotation
            glm::mat4 newViewMatrix = glm::translate(glm::mat4(1.0f), newPosition) * glm::mat4_cast(originalRotation);

            // Set the new view matrix
            camera->setFromViewMatrix(newViewMatrix);
        }
    }
}

void engine::EditorHelper::editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity, int windowX, int windowY, int windowWidth, int windowHeight)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;


    if (editTransformDecomposition)
    {
        ImGui::BeginGroup();
        addIconButton("translate", []() { mCurrentGizmoOperation = ImGuizmo::TRANSLATE; });
        ImGui::SameLine();
        addIconButton("rotate", []() { mCurrentGizmoOperation = ImGuizmo::ROTATE; });
        ImGui::SameLine();
        addIconButton("scale", []() { mCurrentGizmoOperation = ImGuizmo::SCALE; });
        ImGui::EndGroup();

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
        {
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            resetIconToggleStates(); // Turn all off
            setIconToggleState("translate", true); // Turn only this one on
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        {
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
            resetIconToggleStates(); // Turn all off
            setIconToggleState("rotate", true); // Turn only this one on
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        {
            mCurrentGizmoOperation = ImGuizmo::SCALE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("scale", true); // Turn only this one on
        }
    }


    //if (editTransformDecomposition)
    //{
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) // r Key
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;
    //    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;

    //    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    //    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    //    ImGui::InputFloat3("Tr", matrixTranslation);
    //    ImGui::InputFloat3("Rt", matrixRotation);
    //    ImGui::InputFloat3("Sc", matrixScale);
    //    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

    //    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    //    {
    //        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
    //            mCurrentGizmoMode = ImGuizmo::LOCAL;
    //        ImGui::SameLine();
    //        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
    //            mCurrentGizmoMode = ImGuizmo::WORLD;
    //    }
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F10))
    //        useSnap = !useSnap;
    //    ImGui::Checkbox("Snap", &useSnap);
    //    ImGui::SameLine();

    //    switch (mCurrentGizmoOperation)
    //    {
    //    case ImGuizmo::TRANSLATE:
    //        ImGui::InputFloat3("Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::ROTATE:
    //        ImGui::InputFloat("Angle Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::SCALE:
    //        ImGui::InputFloat("Scale Snap", &snap[0]);
    //        break;
    //    }
    //    ImGui::Checkbox("Bound Sizing", &boundSizing);
    //    if (boundSizing)
    //    {
    //        ImGui::PushID(3);
    //        ImGui::Checkbox("", &boundSizingSnap);
    //        ImGui::SameLine();
    //        ImGui::InputFloat3("Snap", boundsSnap);
    //        ImGui::PopID();
    //    }
    //}

    ImGuizmo::SetRect(windowX, windowY, windowWidth, windowHeight);
    if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
    {
        float matrixTranslation2[3], matrixRotation2[3], matrixScale2[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation2, matrixRotation2, matrixScale2);

        auto ttt = Transform{ glm::vec3(matrixTranslation2[0],matrixTranslation2[1], matrixTranslation2[2]), glm::vec3(matrixScale2[0], matrixScale2[1], matrixScale2[2]), glm::vec3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2]) };
        entity->setTransform(ttt);
        entity->updateSelfAndChild();
    }
}
