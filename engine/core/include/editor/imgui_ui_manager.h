#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "imgui_element.h"

namespace engine
{
    class ImGuiUIManager final
    {
    public:
        ImGuiUIManager() = default;
        ~ImGuiUIManager() = default;

        // Register a new UI element
        template<typename T, typename... Args>
        T* create(Args&&... args)
        {
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T* raw = ptr.get();
            m_elements.push_back(std::move(ptr));
            return raw;
        }

        // Remove an element
        void remove(ImGuiElement* element)
        {
            m_elements.erase(
                std::remove_if(m_elements.begin(), m_elements.end(),
                    [&](const std::unique_ptr<ImGuiElement>& e) {
                        return e.get() == element;
                    }),
                m_elements.end()
            );
        }

        // Render all elements
        void render()
        {
            std::sort(m_elements.begin(), m_elements.end(),
                [](const auto& a, const auto& b)
                {
                    return static_cast<int>(a->getCategory()) < static_cast<int>(b->getCategory());
                });

            for (auto& e : m_elements)
                e->render();
        }

        // Toggle visibility by name
        void toggle(const std::string& name)
        {
            if (auto* e = find(name))
                e->show(!e->isVisible());
        }

        // Find element by name
        ImGuiElement* find(const std::string& name)
        {
            for (auto& e : m_elements)
                if (e->getName() == name)
                    return e.get();
            return nullptr;
        }

    private:
        std::vector<std::unique_ptr<ImGuiElement>> m_elements;
    };
}
