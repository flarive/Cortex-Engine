#include "../../include/editor/imgui_editor.h"

#include <string>
#include <format>

// test
#include "../../include/editor/windows/dialog_box.h"
#include "../../include/editor/widgets/custom_widget.h"

#include "../../include/editor/dockspaces/dockspace.h"
#include "../../include/editor/windows/about_window.h"
#include "../../include/editor/windows/scene_window.h"
#include "../../include/editor/windows/settings_window.h"
#include "../../include/editor/windows/properties_window.h"
#include "../../include/editor/windows/floating_toolbar_window.h"


#include "../../include/managers/entity_manager.h"

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
}

/// <summary>
/// Using imGui docking branch
/// https://github.com/ocornut/imgui/issues/2109#issuecomment-430096134
/// </summary>
/// <param name="show"></param>
void engine::ImGuiEditor::renderEditor(bool show, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo)
{
    // renders editor UI
    m_ui.render();

    renderGuizmo(ImGui::GetID("MyDockspace"), projection, view, displayObjectTransformGuizmo);
}

void engine::ImGuiEditor::initRenderGuizmo(const std::shared_ptr<Camera> camera)
{
    m_guizmoCamera = camera;
    camDistance = camera->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));
}

#endif

void engine::ImGuiEditor::renderGuizmo(const ImGuiID& dockspace_id, glm::mat4& projection, glm::mat4& view, const bool displayObjectTransformGuizmo)
{
    if (!m_guizmoCamera)
        return;

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


    ImGuizmo::BeginFrame();

    if (displayObjectTransformGuizmo)
    {
        ImGuizmo::SetOrthographic(!m_guizmoCamera->getIsPerspective());

        // Render the Editor window (no-decoration, for gizmo)
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

        // Remove tab from dock panel
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        ImGui::Begin("FloatingToolbar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
       
        if (m_selectedEntity && m_selectedEntity->name != EntityManager::ROOT_ENTITY_NAME)
        {
            glm::mat4& objectMatrix = m_selectedEntity->getWorldTransform();
            float* objectMatrixPtr = glm::value_ptr(objectMatrix);

            for (int matId = 0; matId < gizmoCount; matId++)
            {
                ImGuizmo::SetID(matId);

                editTransform(viewPtr, projectionPtr2, glm::value_ptr(objectMatrix[matId]), lastUsing == matId, m_selectedEntity);
                if (ImGuizmo::IsUsing())
                {
                    lastUsing = matId;
                }
            }
        }

        ImGui::End();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
    }
}

void engine::ImGuiEditor::editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity)
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
        EditorHelper::beginCenteredToolbar(3, 32);
        EditorHelper::addToolbarIconButton("translate", EditorIcon::editor_translate, []() { mCurrentGizmoOperation = ImGuizmo::TRANSLATE; });
        ImGui::SameLine();
        EditorHelper::addToolbarIconButton("rotate", EditorIcon::editor_rotate, []() { mCurrentGizmoOperation = ImGuizmo::ROTATE; });
        ImGui::SameLine();
        EditorHelper::addToolbarIconButton("scale", EditorIcon::editor_scale, []() { mCurrentGizmoOperation = ImGuizmo::SCALE; });
        EditorHelper::endCenteredToolbar();

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
        {
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("translate", true); // Turn only this one on
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        {
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
            EditorHelper::resetIconToggleStates(); // Turn all off
            EditorHelper::setIconToggleState("rotate", true); // Turn only this one on
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

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
    {
        float matrixTranslation2[3], matrixRotation2[3], matrixScale2[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation2, matrixRotation2, matrixScale2);

        auto ttt = Transform{ glm::vec3(matrixTranslation2[0],matrixTranslation2[1], matrixTranslation2[2]), glm::vec3(matrixScale2[0], matrixScale2[1], matrixScale2[2]), glm::vec3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2]) };
        entity->setTransform(ttt);
        entity->updateSelfAndChild();
    }
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


