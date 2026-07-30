#include "../../../include/editor/windows/properties_window.h"

#include "../../../include/editor/editor_helper.h"



void engine::PropertiesWindow::onInit()
{
    // listen for events from scene hierarchy window
    listen([this](const UIEvent& evt)
    {
        if (evt.sender == "Scene" &&evt.type == UIEventType::EntitySelectionChanged)
        {
            m_selectedEntity = std::any_cast<std::shared_ptr<Entity>>(evt.value);
        }
    });


    m_customWidget.init();
}

void engine::PropertiesWindow::renderPropertiesWidget()
{
    if (m_selectedEntity)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f); // Set rounding to 5 pixels
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f)); // 10 pixels padding on x and y
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));
        ImGui::BeginChild("EntityPropertyRegion", ImVec2(0, 0), true, ImGuiWindowFlags_None);
        displayEntityDetails(m_selectedEntity);
        ImGui::EndChild();
        ImGui::PopStyleVar(3); // Restore default
    }
}

void engine::PropertiesWindow::displayEntityDetails(const std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        auto entityType = entity->getType();
        auto uv = EditorHelper::getEntityTypeMediumIcon(EditorHelper::convertEntityTypeToAtlasIcon(entityType, 48));
        GLuint tex = EditorHelper::getIconAtlasTexture();

        IM_ASSERT(tex != 0);

        // Draw the icon
        ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(48, 48), uv.uv0, uv.uv1);

        // Place next content on the same line as the image
        ImGui::SameLine();

        // Create a vertical group next to the image
        ImGui::BeginGroup();

        // Entity name with color
        ImGui::PushStyleColor(ImGuiCol_Text, EditorHelper::getEntityColor(entityType));
        ImGui::Text("%s", entity->name.c_str());
        ImGui::PopStyleColor();

        // Type name directly below
        ImGui::Text("%s", entity->getTypeNameEx().c_str());

        ImGui::EndGroup();

        renderComponents(entity);
    }
}

void engine::PropertiesWindow::renderComponents(const std::shared_ptr<Entity>& entity)
{
    std::shared_ptr<TransformComponent> transformComponent{};

    // looping over entity components
    for (auto& [typeID, component] : entity->components)
    {
        if (typeID == ComponentType::transform)
        {
            // transform component
            transformComponent = std::reinterpret_pointer_cast<TransformComponent>(component);
            renderTransformComponent(entity);
        }
        else if (typeID == ComponentType::camera)
        {
            // camera component
            auto cameraComponent = dynamic_pointer_cast<CameraComponent>(component);
            if (cameraComponent) renderCameraComponent(cameraComponent);
        }
        else if (typeID == ComponentType::light)
        {
            // light component
            auto lightComponent = dynamic_pointer_cast<LightComponent>(component);
            if (lightComponent) renderLightComponent(lightComponent);
        }
        else if (typeID == ComponentType::model)
        {
            // model component
            auto modelComponent = dynamic_pointer_cast<ModelComponent>(component);
            if (modelComponent) renderModelComponent(modelComponent);
        }
        else if (typeID == ComponentType::primitive)
        {
            // primitive component
            auto primitiveComponent = dynamic_pointer_cast<PrimitiveComponent>(component);
            if (primitiveComponent) renderPrimitiveComponent(primitiveComponent);
        }
        else if (typeID == ComponentType::animator)
        {
            // animator component
            auto animatorComponent = dynamic_pointer_cast<AnimatorComponent>(component);
            if (animatorComponent) renderAnimatorComponent(animatorComponent);
        }
        else if (typeID == ComponentType::particleSystem)
        {
            // particle system component
            auto particleSystemComponent = dynamic_pointer_cast<ParticleSystemComponent>(component);
            if (particleSystemComponent) renderParticleSystemComponent(particleSystemComponent);
        }
        else if (typeID == ComponentType::terrain)
        {
            // terrain component
            auto terrainComponent = dynamic_pointer_cast<TerrainComponent>(component);
            if (terrainComponent) renderTerrainComponent(terrainComponent);
        }
    }
}

void engine::PropertiesWindow::renderTransformComponent(const std::shared_ptr<Entity>& entity)
{
    auto transformComponent = entity->getComponent<TransformComponent>();

    bool displayPosition{ true };
    bool displayRotation{ true };
    bool displayScale{ true };

    if (!transformComponent)
        return;

    //static bool isHeaderChecked = true;
    static bool isHeaderExpanded = true; // Set to true to start expanded

    bool enabled = transformComponent->isEnabled();

    std::function<void(bool)> onCheck = [transformComponent, &enabled](bool checked) {
        transformComponent->setEnabled(checked);
        enabled = transformComponent->isEnabled();
        };


    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(transformComponent->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        glm::vec3 position{ 0,0,0 };
        glm::vec3 rotation{ 0,0,0 };
        glm::vec3 scale{ 1,1,1 };

        std::shared_ptr<CameraComponent> cameraComponent{};
        std::shared_ptr<LightComponent> lightComponent{};
        std::shared_ptr<PrimitiveComponent> primitiveComponent{};
        std::shared_ptr<ModelComponent> modelComponent{};
        std::shared_ptr<ParticleSystemComponent> particleSystemComponent{};
        std::shared_ptr<TerrainComponent> terrainComponent{};

        if (cameraComponent = entity->getComponent<CameraComponent>())
        {
            position = cameraComponent->getCamera()->position;
            displayRotation = false;
            displayScale = false;
        }
        else if (lightComponent = entity->getComponent<LightComponent>())
        {
            position = lightComponent->getLight()->getPosition();
            displayRotation = false;
            displayScale = false;
        }
        else if (primitiveComponent = entity->getComponent<PrimitiveComponent>())
        {
            position = primitiveComponent->getPrimitive()->getPosition();
            scale = primitiveComponent->getPrimitive()->getScale();
            rotation = primitiveComponent->getPrimitive()->getRotation();
        }
        else if (modelComponent = entity->getComponent<ModelComponent>())
        {
            position = modelComponent->getModel()->getPosition();
            scale = modelComponent->getModel()->getScale();
            rotation = modelComponent->getModel()->getRotation();
        }
        else if (particleSystemComponent = entity->getComponent<ParticleSystemComponent>())
        {
            position = particleSystemComponent->getParticleSystem()->getPosition();
            scale = particleSystemComponent->getParticleSystem()->getScale();
            rotation = particleSystemComponent->getParticleSystem()->getRotation();
        }
        else if (terrainComponent = entity->getComponent<TerrainComponent>())
        {
            position = terrainComponent->getTerrain()->getPosition();
            scale = terrainComponent->getTerrain()->getScale();
            rotation = terrainComponent->getTerrain()->getRotation();
        }

        // Local variables for ImGui
        float posX = position.x;
        float posY = position.y;
        float posZ = position.z;

        float rotX = rotation.x;
        float rotY = rotation.y;
        float rotZ = rotation.z;

        float scaX = scale.x;
        float scaY = scale.y;
        float scaZ = scale.z;


        if (ImGui::BeginTable("MyTable", 4, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, EditorHelper::ITEM_LABEL_WIDTH);
            ImGui::TableSetupColumn("vx", ImGuiTableColumnFlags_WidthFixed, 74.0f);
            ImGui::TableSetupColumn("vy", ImGuiTableColumnFlags_WidthFixed, 74.0f);
            ImGui::TableSetupColumn("vz", ImGuiTableColumnFlags_WidthFixed, 74.0f);

            if (displayPosition)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Position");

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##posX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &posX, 0.01f)) {
                    position.x = posX;
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##posY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &posY, 0.01f)) {
                    position.y = posY;
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##posZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &posZ, 0.01f)) {
                    position.z = posZ;
                }
            }

            if (displayRotation)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Rotation");

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##rotX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &rotX, 1.0f)) {
                    rotation.x = rotX;
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##rotY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &rotY, 1.0f)) {
                    rotation.y = rotY;
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##rotZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &rotZ, 1.0f)) {
                    rotation.z = rotZ;
                }
            }

            if (displayScale)
            {
                ImGui::TableNextRow();

                static bool scaleLinked = false;

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Scale");
                ImGui::SameLine(98.0f); // align to right side
                EditorHelper::addDiscreetIconButton(scaleLinked, "LockUnlock", EditorIcon::unlocked, EditorIcon::locked, []() {});

                ImGui::TableSetColumnIndex(1);
                if (EditorHelper::drawCustomDragFloat("X", "##scaX", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::green, EditorHelper::white, &scaX, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.x = scaX;
                    }
                    else
                    {
                        scale.x = scaX;
                        scale.y = scaX;
                        scale.z = scaX;
                    }
                }

                ImGui::TableSetColumnIndex(2);
                if (EditorHelper::drawCustomDragFloat("Y", "##scaY", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::red, EditorHelper::white, &scaY, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.y = scaY;
                    }
                    else
                    {
                        scale.y = scaY;
                        scale.x = scaY;
                        scale.z = scaY;
                    }
                }

                ImGui::TableSetColumnIndex(3);
                if (EditorHelper::drawCustomDragFloat("Z", "##scaZ", ImGui::GetCursorScreenPos(), EditorHelper::SIZE, EditorHelper::ROUNDING, 50.0f, EditorHelper::blue, EditorHelper::white, &scaZ, 0.01f)) {
                    if (!scaleLinked)
                    {
                        scale.z = scaZ;
                    }
                    else
                    {
                        scale.z = scaZ;
                        scale.x = scaZ;
                        scale.y = scaZ;
                    }
                }
            }

            ImGui::EndTable();
        }

        if (cameraComponent)
        {
            cameraComponent->getCamera()->position = position;
        }
        else if (lightComponent)
        {
            lightComponent->getLight()->setPosition(position);
        }
        else if (primitiveComponent)
        {
            auto p = primitiveComponent->getPrimitive();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (modelComponent)
        {
            auto p = modelComponent->getModel();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (particleSystemComponent)
        {
            auto p = particleSystemComponent->getParticleSystem();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }
        else if (terrainComponent)
        {
            auto p = terrainComponent->getTerrain();
            p->setPosition(position);
            p->setRotation(rotation);
            p->setScale(scale);
        }

        updateTransformComponent(transformComponent, position, rotation, scale); // dirty
    }
}


void engine::PropertiesWindow::renderLightComponent(std::shared_ptr<LightComponent>& component)
{
    auto light = component->getLight();
    if (!light)
        return;


    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(light->getTypeID()));
    }
}

void engine::PropertiesWindow::renderCameraComponent(std::shared_ptr<CameraComponent>& component)
{
    auto camera = component->getCamera();
    if (!camera)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::
        collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(camera->getTypeID()));
    }
}

void engine::PropertiesWindow::renderPrimitiveComponent(std::shared_ptr<PrimitiveComponent>& component)
{
    auto primitive = component->getPrimitive();
    if (!primitive)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(primitive->getTypeID()));
    }


    // material
    m_customWidget.render();
}

void engine::PropertiesWindow::renderModelComponent(std::shared_ptr<ModelComponent>& component)
{
    auto model = component->getModel();
    if (!model)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(model->getTypeID()));
    }
}

void engine::PropertiesWindow::renderAnimatorComponent(std::shared_ptr<AnimatorComponent>& component)
{
    auto animator = component->getAnimator();
    if (!animator)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(animator->getTypeID()));
    }
}

void engine::PropertiesWindow::renderParticleSystemComponent(std::shared_ptr<ParticleSystemComponent>& component)
{
    auto particleSystem = component->getParticleSystem();
    if (!particleSystem)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(particleSystem->getTypeID()));
    }
}

void engine::PropertiesWindow::renderTerrainComponent(std::shared_ptr<TerrainComponent>& component)
{
    auto terrain = component->getTerrain();
    if (!terrain)
        return;

    bool enabled = component->isEnabled();

    std::function<void(bool)> onCheck = [component, &enabled](bool checked) {
        component->setEnabled(checked);
        enabled = component->isEnabled();
        };

    static bool isHeaderExpanded = true; // Set to true to start expanded

    ImGui::SetNextItemOpen(isHeaderExpanded, ImGuiCond_Once);
    if (EditorHelper::collapsingCheckboxHeader(component->getName().c_str(), &enabled, ImGuiTreeNodeFlags_None, onCheck))
    {
        EditorHelper::renderDynamicProperties(component, to_string(terrain->getTypeID()));
    }
}

void engine::PropertiesWindow::updateTransformComponent(std::shared_ptr<TransformComponent>& transformComponent, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
    auto trs = transformComponent->getTransform();
    trs.setLocalPosition(position);
    trs.setLocalRotation(rotation);
    trs.setLocalScale(scale);
    transformComponent->setTransform(trs);
}
