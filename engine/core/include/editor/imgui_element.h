#pragma once

#include <imgui.h>
#include <string>
#include <vector>

namespace engine
{
    class ImGuiElement
    {
    public:
        enum class Type
        {
            Window,
            Overlay,
            Widget
        };

        ImGuiElement(Type type, const std::string& name)
            : m_type(type), m_name(name), m_visible(true)
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


        Type getType() const { return m_type; }
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
            switch (m_type)
            {
            case Type::Window:
                ImGui::Begin(m_name.c_str());
                break;

            case Type::Overlay:
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

            case Type::Widget:
                // Widgets do not create windows
                break;
            }
        }

        virtual void draw() = 0;

        virtual void end()
        {
            if (m_type == Type::Window || m_type == Type::Overlay)
                ImGui::End();
        }


    protected:
        Type m_type;
        std::string m_name;
        bool m_visible;

        std::vector<ImFont*> m_fontStack;
        std::vector<ImGuiCol> m_colorStack;
    };
}