#pragma once

#include "../imgui_element.h"

#include "../../../core/include/ecs/entity.h"

namespace engine
{
    class SceneWindow final : public ImGuiElement
    {
    public:
        SceneWindow() : ImGuiElement(Category::Window, "Scene") {}
        ~SceneWindow() = default;
        
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
        void draw(glm::mat4& projection, glm::mat4& view) override
        {
            renderHierarchyWidget();
        }
    };
}