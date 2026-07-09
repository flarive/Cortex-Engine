#include "../../include/editor/imgui_element.h"

#include "../../include/editor/imgui_ui_manager.h"

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
    }
}

void engine::ImGuiElement::end()
{
    if (m_category == Category::Window || m_category == Category::Overlay)
        ImGui::End();
}