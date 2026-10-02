#include "../../include/windows/floating_toolbar_window.h"



// https://github.com/TheCherno/ImGuizmo
#include "extensions/imGuizmo/ImGuizmo.h"

#include "../../../editor/include/editor_helper.h"
#include "../../../core/include/managers/entity_manager.h"


#include <glm/gtc/type_ptr.hpp>


#include <imgui_internal.h>


void engine::FloatingToolbarWindow::init()
{
    // listen for events from scene hierarchy window
    listen([this](const UIEvent& evt)
        {
            if (evt.sender == "Scene" && evt.type == UIEventType::EntitySelectionChanged)
            {
                auto entity = std::any_cast<std::shared_ptr<Entity>>(evt.value);
                m_selectedEntity = entity;   // store as weak_ptr
            }
        });
}


void engine::FloatingToolbarWindow::setCamera(std::shared_ptr<Camera> camera)
{
    m_guizmoCamera = camera;
    camDistance = camera->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));
}

void engine::FloatingToolbarWindow::setSelectedEntity(std::shared_ptr<Entity> selectedEntity)
{
    m_selectedEntity = selectedEntity;
}

void engine::FloatingToolbarWindow::renderGuizmo(glm::mat4& projection, glm::mat4& view)
{
    if (!m_guizmoCamera)
        return;

    //ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

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


    //ImGui::SetNextWindowPos(ImVec2(500, 200), ImGuiCond_Always);

    ImGuizmo::BeginFrame();

    /*if (displayObjectTransformGuizmo)
    {*/
        ImGuizmo::SetOrthographic(!m_guizmoCamera->getIsPerspective());

        //// Render the Editor window (no-decoration, for gizmo)
        //ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

        // Remove tab from dock panel
        //ImGuiWindowClass window_class;
        //window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        //ImGui::SetNextWindowClass(&window_class);

        //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        //ImGui::Begin("FloatingToolbar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        //

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

        //ImGui::End();

        //ImGui::PopStyleVar(1);
        //ImGui::PopStyleColor(1);
    //}
}

void engine::FloatingToolbarWindow::editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity)
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