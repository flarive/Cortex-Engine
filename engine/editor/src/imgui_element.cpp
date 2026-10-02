#include "../include/imgui_element.h"

#include "../include/imgui_ui_manager.h"

#include <imgui_internal.h>


//// https://github.com/TheCherno/ImGuizmo
//#include "extensions/imGuizmo/ImGuizmo.h"

engine::ImGuiElement::ImGuiElement(Category category, const std::string& name)
    : m_category(category), m_name(name), m_visible(true), m_manager(nullptr)
{
}

void engine::ImGuiElement::setManager(ImGuiUIManager* mgr)
{
    m_manager = mgr;
}

void engine::ImGuiElement::emit(UIEventType type, const std::string& param, std::any payload)
{
    if (m_manager)
        m_manager->emitEvent({ type, m_name, param, payload });
}

void engine::ImGuiElement::listen(EventCallback cb)
{
    if (m_manager)
        m_manager->addListener(cb);
}

void engine::ImGuiElement::begin()
{
    switch (m_category)
    {
    case Category::Window:
        ImGui::Begin(m_name.c_str());
        break;

    case Category::Overlay:
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        ImGui::Begin(m_name.c_str(),
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav);
        break;

    case Category::Widget:
        // Widgets do not create windows
        break;

    case Category::FloatingWindow:
        //ImGuizmo::BeginFrame();
        
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");

        // Render the Editor window (no-decoration, for gizmo)
        ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);

        // Remove tab from dock panel
        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
        ImGui::SetNextWindowClass(&window_class);

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

        ImGui::Begin("FloatingToolbar", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        break;
    }
}

void engine::ImGuiElement::end()
{
    switch (m_category)
    {
    case Category::Window:
        ImGui::End();
        break;

    case Category::Overlay:
        ImGui::End();
        break;

    case Category::FloatingWindow:
        ImGui::End();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);
        break;
    }
}