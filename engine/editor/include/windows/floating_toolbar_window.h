#pragma once

#include <imgui.h>

#include "../imgui_element.h"

#include "../../../core/include/ecs/entity.h"
#include "../../../core/include/cameras/camera.h"

namespace engine
{
    class FloatingToolbarWindow final : public ImGuiElement
    {
    public:
        FloatingToolbarWindow() : ImGuiElement(Category::FloatingWindow, "FloatingToolbar") {}
        ~FloatingToolbarWindow() = default;

        void init() override;

        void setCamera(std::shared_ptr<Camera> camera);

        void setSelectedEntity(std::shared_ptr<Entity> selectedEntity);

    private:
        
        float camDistance{};
        int gizmoCount{ 1 };

        
        int lastUsing{};


        std::shared_ptr<Entity> m_selectedEntity{};
        std::shared_ptr<Camera> m_guizmoCamera{};



        void renderGuizmo(glm::mat4& projection, glm::mat4& view);
        void editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity);

    protected:
        void draw(glm::mat4& projection, glm::mat4& view) override
        {
            renderGuizmo(projection, view);
        }
    };
}