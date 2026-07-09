#pragma once

#include "imgui_element.h"

#include "../ecs/entity.h"
#include "../ecs/transform_component.h"
#include "../ecs/light_component.h"
#include "../ecs/camera_component.h"
#include "../ecs/model_component.h"
#include "../ecs/primitive_component.h"
#include "../ecs/animator_component.h"
#include "../ecs/particlesystem_component.h"
#include "../ecs/terrain_component.h"

namespace engine
{
    class PropertiesWindow final : public ImGuiElement
    {
    public:
        PropertiesWindow() : ImGuiElement(Category::Window, "Properties") {}

        void onInit() override;


    private:
        std::shared_ptr<Entity> m_selectedEntity{};

        void renderPropertiesWidget();
        void displayEntityDetails(const std::shared_ptr<Entity>& entity);

        void renderComponents(const std::shared_ptr<Entity>& entity);

        void renderTransformComponent(const std::shared_ptr<Entity>& entity);
        void renderLightComponent(std::shared_ptr<LightComponent>& component);
        void renderCameraComponent(std::shared_ptr<CameraComponent>& component);
        void renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component);
        void renderModelComponent(std::shared_ptr<ModelComponent>& component);
        void renderAnimatorComponent(std::shared_ptr<AnimatorComponent>& component);
        void renderParticleSystemComponent(std::shared_ptr<ParticleSystemComponent>& component);
        void renderTerrainComponent(std::shared_ptr<TerrainComponent>& component);

        void updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

    protected:
        void draw() override
        {
            renderPropertiesWidget();
        }
    };
}