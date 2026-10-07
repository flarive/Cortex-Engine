#include "../include/imgui_editor.h"

#include <string>
#include <format>

// test
#include "../include/windows/dialog_box.h"


#include "../include/dockspaces/dockspace.h"
#include "../include/windows/about_window.h"
#include "../include/windows/scene_window.h"
#include "../include/windows/settings_window.h"
#include "../include/windows/properties_window.h"
#include "../include/windows/floating_toolbar_window.h"


#include "../../core/include/managers/entity_manager.h"

// https://github.com/TheCherno/ImGuizmo
#include "extensions/imGuizmo/ImGuizmo.h"



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui_internal.h>

#include <unordered_map>

#if EDITOR_MODE

void engine::ImGuiEditor::init()
{
    EditorHelper::registerIconAtlas();

    // register to imGui UI manager events
    m_ui.addListener([this](const UIEvent& evt)
    {
        onEditorUIEvent(evt);
    });
}

void engine::ImGuiEditor::setScene(std::shared_ptr<Entity> rootEntity)
{
    m_rootEntity = rootEntity;
    m_selectedEntity = rootEntity;
}

void engine::ImGuiEditor::initEditor()
{
    // create dockspace (editor layout)
    m_ui.create<DockSpaceElement>();

    // create editor windows and add them in the dockspace
    m_ui.create<SceneWindow>()->setRootEntity(m_rootEntity);
    m_ui.create<SettingsWindow>();
    m_ui.create<AboutWindow>()->init();
    m_ui.create<PropertiesWindow>();

    if (auto toolbar = m_ui.create<FloatingToolbarWindow>())
    {
        toolbar->setCamera(m_guizmoCamera);
        toolbar->setSelectedEntity(m_selectedEntity);
    }
}

/// <summary>
/// Using imGui docking branch
/// https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
/// </summary>
/// <param name="show"></param>
void engine::ImGuiEditor::renderEditor(bool show, glm::mat4& projection, glm::mat4& view)
{
    // renders editor UI
    m_ui.render(projection, view);
}

void engine::ImGuiEditor::initRenderGuizmo(const std::shared_ptr<Camera> camera)
{
    assert(camera != nullptr);

    m_guizmoCamera = camera;
    camDistance = camera->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));
}

void engine::ImGuiEditor::renderViewGuizmo(glm::mat4& projection, glm::mat4& view, bool displayViewTransformGuizmo)
{
    if (!m_guizmoCamera)
        return;

    if (displayViewTransformGuizmo)
    {
        ImGuiIO& io = ImGui::GetIO();

        // Calculate the guizmo position relative to the window's top-right corner
        ImVec2 pos = ImVec2(io.DisplaySize.x - 128.0f, 0.0f);
        ImVec2 size = ImVec2(128, 128);

        // MUTUALIZE !!!!!
        ImGuizmo::BeginFrame();

        // Convert glm::mat4 to const float*
        const float* projectionPtr = glm::value_ptr(projection);
        const float* viewPtr = glm::value_ptr(view);

        float* projectionPtr2 = glm::value_ptr(projection);
        float* viewPtr2 = glm::value_ptr(view);

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
            m_guizmoCamera->setFromViewMatrix(newViewMatrix);
        }
    }
}

void engine::ImGuiEditor::onEditorUIEvent(const UIEvent& evt)
{
    if (evt.type == UIEventType::EntitySelectionChanged)
    {
        m_selectedEntity = std::any_cast<std::shared_ptr<Entity>>(evt.value);

        if (m_onSelectionChanged)
            m_onSelectionChanged(m_selectedEntity);
    }
    else if (evt.type == UIEventType::SceneSettingChanged)
    {
        if (m_onSceneSettingChanged)
        {
            // convert std::any to SceneSetting std::variant
            if (any_is<bool>(evt.value))
            {
                bool v = std::any_cast<bool>(evt.value);
                m_onSceneSettingChanged(evt.key, v);
            }
            else if (any_is<int>(evt.value))
            {
                int v = std::any_cast<int>(evt.value);
                m_onSceneSettingChanged(evt.key, v);
            }
            else if (any_is<uint>(evt.value))
            {
                uint v = std::any_cast<uint>(evt.value);
                m_onSceneSettingChanged(evt.key, v);
            }
            else if (any_is<ubyte>(evt.value))
            {
                ubyte v = std::any_cast<ubyte>(evt.value);
                m_onSceneSettingChanged(evt.key, v);
            }
            else if (any_is<float>(evt.value))
            {
                float v = std::any_cast<float>(evt.value);
                m_onSceneSettingChanged(evt.key, v);
            }
        }
    }

    // You can handle more event types here
}
#endif