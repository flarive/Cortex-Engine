#pragma once

#include <imgui.h>
#include <string>
#include <vector>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "extensions/toggle/imgui_toggle.h"
#include "extensions/toggle/imgui_toggle_palette.h"

namespace engine
{
    enum class Category
    {
        DockSpace = 0,
        Window = 1,
        Widget = 2,
        Overlay = 3
    };

    class ImGuiElement
    {
    public:
        
        ImGuiElement(Category category, const std::string& name)
            : m_category(category), m_name(name), m_visible(true)
        {}

        virtual ~ImGuiElement() = default;

        // Main entry point called by your UI manager
        void render()
        {
            if (!m_visible)
                return;

            begin();
            draw();
            end();
        }

        // Visibility control
        void show(bool v = true) { m_visible = v; }


        Category getCategory() const { return m_category; }
        std::string getName() const { return m_name; }
        bool isVisible() const { return m_visible; }

        // Optional: font helpers
        void pushFont(ImFont* font)
        {
            if (font)
            {
                ImGui::PushFont(font);
                m_fontStack.push_back(font);
            }
        }

        void popFont()
        {
            if (!m_fontStack.empty())
            {
                ImGui::PopFont();
                m_fontStack.pop_back();
            }
        }

        // Optional: style helpers
        void pushColor(ImGuiCol idx, const ImVec4& col)
        {
            ImGui::PushStyleColor(idx, col);
            m_colorStack.push_back(idx);
        }

        void popColor()
        {
            if (!m_colorStack.empty())
            {
                ImGui::PopStyleColor();
                m_colorStack.pop_back();
            }
        }

    protected:
        // Derived classes override these
        virtual void begin()
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

        virtual void draw() = 0;

        virtual void end()
        {
            if (m_category == Category::Window || m_category == Category::Overlay)
                ImGui::End();
        }


    protected:
        Category m_category;
        std::string m_name;
        bool m_visible;

        std::vector<ImFont*> m_fontStack;
        std::vector<ImGuiCol> m_colorStack;
    };
}