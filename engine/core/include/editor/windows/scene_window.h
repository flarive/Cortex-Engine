#pragma once

#include "../imgui_element.h"

#include "../../ecs/entity.h"

namespace engine
{
    class SceneWindow final : public ImGuiElement
    {
    public:
        SceneWindow() : ImGuiElement(Category::Window, "Scene") {}
        
        void setRootEntity(const std::shared_ptr<Entity>& entity)
        {
            m_rootEntity = entity;   // stored as weak_ptr
        }

    private:
        std::weak_ptr<Entity> m_rootEntity{};
        std::weak_ptr<Entity> m_selectedEntity{};

        void renderHierarchyWidget();
        void displayEntityHierarchy(const std::shared_ptr<Entity>& entity);

    protected:
        void draw() override
        {
            renderHierarchyWidget();
        }
    };
}