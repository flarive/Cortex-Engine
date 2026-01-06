#include "../../include/app/scene.h"

#include "extensions/imGuizmo/ImGuizmo.h"
#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"

#include "../../include/debug/opengl_debug.h"

#include "../../include/tools/helpers.h"
#include "../../include/aabb.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "themes/imgui_spectrum.h"

#include <functional>
#include <memory>
#include <iostream>
#include <limits>
#include <format>


namespace engine {
    Scene* Scene::currentInstance = nullptr; // Definition
}

using Clock = std::chrono::high_resolution_clock;


engine::Scene::Scene(std::string _title, App* _app, SceneSettings _settings)
    : title(_title), app(_app)
{
    if (_settings.method == RenderMethod::PBR) {
        m_renderer = new PbrRenderer(app->window);
    }
    else {
        m_renderer = new BlinnPhongRenderer(app->window);
    }

    // create scene entities hierarchy
    m_entityManager.create();

    // store SceneSettings in a singleton for easy access everywhere
    engine::Singleton::initialize(_settings);

    currentInstance = this; // Set the current instance when constructing
}

void engine::Scene::before_init()
{
    before_init_internal();     // Always called
    before_init_hook();         // Hook for derived logic
}

void engine::Scene::after_init()
{
    after_init_internal();     // Always called
    after_init_hook();         // Hook for derived logic
}

void engine::Scene::before_init_internal()
{
    // Always run this code

	// be notified when audio manager is initialized and ready
    m_audioManager.setInitCallback([](bool success) {
        if (success) {
			logger.info("OpenAL initialized successfully!");
            // Proceed with audio operations
        }
        else {
			logger.error("OpenAL initialization failed!");
        }
     });
}

void engine::Scene::after_init_internal()
{
    // Always run this code
    initEntities();
    
    // Counters
    unsigned short spotLightCount = 0, dirLightCount = 0, pointLightCount = 0, areaLightCount = 0;

    // Count each type using dynamic_pointer_cast
    for (const auto& light : lights) {
        if (std::dynamic_pointer_cast<SpotLight>(light)) {
            ++spotLightCount;
        }
        else if (std::dynamic_pointer_cast<DirectionalLight>(light)) {
            ++dirLightCount;
        }
        else if (std::dynamic_pointer_cast<PointLight>(light)) {
            ++pointLightCount;
        }
        else if (std::dynamic_pointer_cast<AreaLight>(light)) {
            ++areaLightCount;
        }
    }

    m_renderer->setLightsCount(pointLightCount, dirLightCount, spotLightCount, areaLightCount);

    // Fill imGui debug window with current scene hierarchy
    #if EDITOR_MODE
    m_editor.setScene(m_entityManager.getRootEntity());
    #endif
    
    // count all items in the scene
    countItems(m_entityManager.getRootEntity());

    initQueries();
}

void engine::Scene::initialize()
{
    before_init();

    init();

    lights = m_entityManager.findEntitiesOfType<Light>();
	if (lights.size() == 0) logger.warn("Scene has no light !");
    //assert(lights.size() > 0 && "Scene has no light !");

    // assign light indexes
    computeLightsIndexes();

    cameras = m_entityManager.findEntitiesOfType<Camera>();
    if (cameras.size() == 0) logger.warn("Scene has no camera !");
    //assert(cameras.size() > 0 && "Scene has no camera !");

	camDistance = getActiveCamera()->getDistanceToTarget(glm::vec3(0.0f, -0.35f, 0.0f));

    // renderer setup
    m_renderer->setup(app->width, app->height, getActiveCamera(), lights);

    // listen for editor selected entity changed
    #if EDITOR_MODE
    listenForEditorChanges();
    #endif

    // mouse picking for editor mode
    //if (show_demo_window)
        //glfwSetMouseButtonCallback(app->window, mouseButtonCallback);

    EditorHelper::setIconToggleState("translate", true);
    EditorHelper::setIconToggleState("rotate", false);
    EditorHelper::setIconToggleState("scale", false);


    after_init();
}

// Callback function
void engine::Scene::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get mouse position and perform ray casting
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (currentInstance)
        {
			currentInstance->performRayCasting(xpos, ypos);
        }
    }
}



void engine::Scene::computeLightsIndexes()
{
    unsigned int nextPointLightIndex{};
    unsigned int nextDirLightIndex{};
    unsigned int nextSpotLightIndex{};
    unsigned int nextAreaLightIndex{};

    for (const auto& light : lights)
    {
        if (light)
        {
            if (light->getTypeID() == LightType::point)
            {
                light->setIndex(nextPointLightIndex);
                nextPointLightIndex++;
            }
            else if (light->getTypeID() == LightType::directional)
            {
                light->setIndex(nextDirLightIndex);
                nextDirLightIndex++;
            }
            else if (light->getTypeID() == LightType::spot)
            {
                light->setIndex(nextSpotLightIndex);
                nextSpotLightIndex++;
            }
            else if (light->getTypeID() == LightType::area)
            {
                light->setIndex(nextAreaLightIndex);
                nextAreaLightIndex++;
            }
        }
    }
}

#if EDITOR_MODE
void engine::Scene::listenForEditorChanges()
{
    m_editor.setOnSelectionChanged([this](std::shared_ptr<Entity> entity)
        {
            logger.info("Selected entity changed: {} (id {})", entity->name, entity->id);
			m_selectedEntity = entity;
        });

    m_editor.setOnSceneSettingChanged([this](std::string key, std::variant<bool, int, unsigned int, float> value)
        {
            auto* singleton = engine::Singleton::getInstance();
            assert(singleton != nullptr && "Singleton not initialized !");
            SceneSettings& sceneSettings = singleton->sceneSettings();

			bool boolValue = false;
			int intValue = 0;
            unsigned int unsignedIntValue = 0;
			float floatValue = 0.0f;

            if (std::holds_alternative<bool>(value))
            {
                boolValue = std::get<bool>(value);
                logger.info("{} setting changed: {}", key, boolValue);
            }
            else if (std::holds_alternative<int>(value))
            {
                intValue = std::get<int>(value);
                logger.info("{} setting changed: {}", key, intValue);
            }
            else if (std::holds_alternative<float>(value))
            {
                floatValue = std::get<float>(value);
                logger.info("{} setting changed: {}", key, floatValue);
            }

            if (key == "draw_wireframe") {
                sceneSettings.drawAsWireframe = boolValue;
            }
            else if (key == "enable_gamma_correction") {
                sceneSettings.enableGammaCorrection = boolValue;
            }
            else if (key == "enable_face_culling") {
                sceneSettings.enableFaceCulling = boolValue;
            }
            else if (key == "enable_camera_frustrum_culling") {
                sceneSettings.enableCameraFrustrumCulling = boolValue;
			}
            else if (key == "draw_lights_visual_helpers") {
                sceneSettings.drawLightsVisualHelpers = boolValue;
            }
            else if (key == "draw_bounding_boxes_visual_helpers") {
                sceneSettings.drawBoundingBoxesVisualHelpers = boolValue;
            }
            else if (key == "draw_debug_normals_visual_helpers") {
                sceneSettings.drawNormalsVisualHelpers = boolValue;
            }
            else if (key == "enable_shadows") {
                sceneSettings.enableShadows = boolValue;
            }
            else if (key == "shadow_calculation_method") {
                sceneSettings.shadowCalculationMethod = intValue;
            }
            else if (key == "shadow_intensity") {
                sceneSettings.shadowIntensity = floatValue;
            }
            else if (key == "shadow_maps_texture_size") {
                sceneSettings.shadowMapsTextureSize = static_cast<float>(intValue);
            }
            else if (key == "shadow_maps_bias_factor") {
                sceneSettings.shadowMapsBiasFactor = floatValue;
            }
            else if (key == "shadow_maps_blur_factor") {
                sceneSettings.shadowMapsBlur = floatValue;
            }
        });
}
#endif

void engine::Scene::gameLoop()
{
    // Start CPU timer
    auto cpuFrameStart = Clock::now();

    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    if (glfwGetWindowAttrib(app->window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }

    // measure ui time (part 1 begin)
    auto uiStart1 = Clock::now();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    framerate = ImGui::GetIO().Framerate;

    // Editor mode windows
    #if EDITOR_MODE
    if (is_editor_mode)
    {
        m_displayObjectTransformGuizmo = true;
        m_displayViewTransformGuizmo = false;
        
        app->setWindowTitle("EDITOR");
        m_editor.renderUIWindow(is_editor_mode);
        renderGuizmo();
    }
    #endif

    
    
    if (show_perf_overlay && !is_editor_mode)
        m_perfOverlay.renderPerfOverlay(&show_perf_overlay, framerate, cpuTime, gpuTime, uiTime);

    // Dear ImGui demo windows
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);
    //


    // measure ui time (part 1 end)
    auto uiEnd1 = Clock::now();
    std::chrono::duration<double, std::milli> uiDuration1 = uiEnd1 - uiStart1;
    uiTime = uiDuration1.count();

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // fps capping (begin)
    std::chrono::steady_clock::time_point start_time{};
    if (app->capFramerate())
    {
        start_time = Clock::now();
    }

    // get opengl stats such as polycount drawn, GPU timer...
    beginQuery();

    // Lambda to update
    auto updateLambda = [this](Shader& shader) {
        // execute scene update
        if (shader.name != "outline")
            update(shader);

        // draw our scene graph
        drawEntities(shader);
        };

    // Lambda to update the UI
    auto updateUILambda = [this]() {
        updateUI();
        };

    // Call the renderer loop
    m_renderer->loop(app->width, app->height, getActiveCamera(), updateLambda, updateUILambda);

    // get opengl stats such as polycount drawn, GPU timer...
    endQuery();

    // measure ui time (part 2 begin)
    auto uiStart2 = Clock::now();

    m_renderer->enableGammaCorrection(false);

    // ImGUI rendering
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(app->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    // For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(app->window);

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    m_renderer->enableGammaCorrection(sceneSettings.enableGammaCorrection);

    // measure ui time (part 2 end)
    auto uiEnd2 = Clock::now();
    std::chrono::duration<double, std::milli> uiDuration2 = uiEnd2 - uiStart2;
    uiTime += uiDuration2.count();

    // fps capping (end)
    if (app->capFramerate())
    {
        auto end_time = Clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(app->getFrameDelay()) - (end_time - start_time));
    }

    // End CPU timer
    auto cpuFrameEnd = Clock::now();
    std::chrono::duration<double, std::milli> cpuFrameDuration = cpuFrameEnd - cpuFrameStart;
    cpuTime = cpuFrameDuration.count();
}

void engine::Scene::initEntities()
{
    // Init using stored world transforms
    initEntityRecursive(m_entityManager.getRootEntity());
}

void engine::Scene::initEntityRecursive(const std::shared_ptr<engine::Entity>& entity)
{
    // new way
    for (const auto& [typeID, component] : entity->components)
    {
        auto trs = entity->getTransform();
        
        if (typeID != ComponentType::transform)
        {
            component->init(trs);
        }
    }

    // init children
    for (const auto& child : entity->children)
    {
        initEntityRecursive(child);
    }
}

void engine::Scene::drawEntities(Shader& shader)
{
    inFrustrumCount = 0;
    totalFrustrumCount = 0;
    
    // draw flat and nested entity hierarchy
    // Precompute transforms for all entities before drawing
    if (shader.name != "outline")
        m_entityManager.getRootEntity()->updateSelfAndChild();


    auto cam = getActiveCamera();
    glm::mat4 projection = cam->getProjectionMatrix(app->width, app->height, 0.1f, 100.0f);
    glm::mat4 view = cam->getViewMatrix();
    const Frustum camFrustum = cam->createFrustumFromCamera(app->width / app->height, glm::radians(cam->zoom), 0.1f, 100.0f);


    // Draw using stored world transforms
    drawEntityRecursive(m_entityManager.getRootEntity(), shader, projection, view, camFrustum);
}

void engine::Scene::drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Frustum& camFrustum)
{
    if (!entity->enabled)
        return;

    bool shouldTestFrustrumForEntity = false;
    bool frustrumOk = false;

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.enableCameraFrustrumCulling)
    {
        if (auto boundingVolume = entity->getBoundingVolume(); boundingVolume != nullptr)
        {
            shouldTestFrustrumForEntity = true;

            // https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
            if (shouldTestFrustrumForEntity && boundingVolume->isOnFrustum(camFrustum, entity->getWorldTransform()))
            {
                frustrumOk = true;
                //std::cout << "Entity " << entity->id << " is inside frustum." << std::endl;
            }
            //else if (shouldTestFrustrum)
            //{
            //    std::cout << "Entity " << entity->id << " is OUTSIDE frustum." << std::endl;

            //    // Log which planes failed
            //    if (boundingVolume)
            //    {
            //        std::cout << "Entity " << entity->id << " is being tested as AABB." << std::endl;
            //        std::cout << "Near plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.nearFace) << std::endl;
            //        std::cout << "Far plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.farFace) << std::endl;
            //        std::cout << "Left plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.leftFace) << std::endl;
            //        std::cout << "Right plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.rightFace) << std::endl;
            //        std::cout << "Top plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.topFace) << std::endl;
            //        std::cout << "Bottom plane test: " << boundingVolume->isOnOrForwardPlane(camFrustum.bottomFace) << std::endl;
            //    }
            //    else {
            //        std::cout << "Entity " << entity->name << " does not have a bounding volume." << std::endl;
            //    }
            //}
        }
        //else
        //{
        //    if (entity->getType() == EntityType::primitive || entity->getType() == EntityType::model)
        //    {
        //        std::cout << "Entity " << entity->name << " does not have a bounding volume." << std::endl;
        //    }
        //}
    }

    if (!shouldTestFrustrumForEntity || (shouldTestFrustrumForEntity && frustrumOk))
    {
        // Use the precomputed transform
        shader.use();

        if (shader.name != "outline")
        {
            glStencilFunc(GL_ALWAYS, entity->id, 0xFF);
            glStencilMask(0xFF);
        }
        else if (m_selectedEntity)
        {
            // Only draw outline where stencil != objectID
            glStencilFunc(GL_NOTEQUAL, entity->id, 0xFF);
            glStencilMask(0x00); // disable stencil writes

            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setFloat("outlineWidth", entity->id == m_selectedEntity->id ? 0.08f : 0.0f);
        }

        auto transform = entity->getTransform();

        // looping over entity components
        for (const auto& [typeID, component] : entity->components)
        {
            if (typeID == ComponentType::primitive || typeID == ComponentType::model)
            {
				bool shouldDraw = true;
                
                // manage shadow casting or not
                if (shader.name == "simpleDepthBuffer1" || shader.name == "simpleDepthBuffer2")
                {
                    auto properties = component->getPublicProperties();
                    if (properties.contains("canCastShadows"))
                    {
                        auto canCastShadows = properties.at("canCastShadows");
                        if (auto pBool = std::get_if<bool>(&canCastShadows.value))
                        {
                            shouldDraw = *pBool;
                        }
                    }
                }
                
                // primitive and model
                if (shouldDraw)
                    component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());

                inFrustrumCount++;
            }
            else if (typeID == ComponentType::light)
            {
                // light
                component->draw(projection, view, shader, entity->getWorldTransform(), transform);
            }
            else if (typeID == ComponentType::animator)
            {
                // camera
				component->update(deltaTime, transform);
                component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());
            }
            else if (typeID == ComponentType::camera)
            {
                // camera
            }
        }

        if (shader.name == "outline")
        {
            // Restore state
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
        }

        // Draw children
        for (const auto& child : entity->children)
        {
            drawEntityRecursive(child, shader, projection, view, camFrustum);
        }
    }

    auto entityType = entity->getType(); // could be optimized/avoided
    if (entityType == EntityType::primitive || entityType == EntityType::model)
    {
        totalFrustrumCount++;
    }
}

void engine::Scene::exit()
{
    glBindVertexArray(0);

    // optional: de-allocate all resources once they've outlived their purpose
    //glDeleteVertexArrays(1, &quadVAO);
    ////glDeleteBuffers(1, &quadVBO);
    //glDeleteRenderbuffers(1, &rbo);
    //glDeleteFramebuffers(1, &colorFramebuffer);
    //glDeleteFramebuffers(1, &depthMapFramebuffer);

    //blinnPhongShader.clean();
    //screenShader.clean();
    //skyboxReflectShader.clean();

    //TODO implement renderer clean & exit !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    m_renderer->clean();
	m_entityManager.clean();
    m_audioManager.clean();

    // clean user stuffs
    clean();

    cleanupQueries(); // called ? to test !!!!!
}

GLFWwindow* engine::Scene::getWindow()
{
    return app->window;
}

void engine::Scene::key_callback(int key, int scancode, int action, int mods)
{
    (void)scancode;   //Do nothing
    (void)mods;   //Do nothing

    // basic window handling
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(app->window, GL_TRUE); break;
    case GLFW_KEY_ENTER:
        if (action == GLFW_RELEASE)
        {
            app->toggleFullscreen([this]() {
                this->refreshFullscreen();
                });
        }
        break;
    case GLFW_KEY_F11:
        if (action == GLFW_PRESS && !key_f11_pressed) {
            is_editor_mode = !is_editor_mode;
            key_f11_pressed = true;
        }
        else if (action == GLFW_RELEASE) {
            key_f11_pressed = false;
        }
        break;
    case GLFW_KEY_SPACE:
        if (action == GLFW_RELEASE)
        {
            show_demo_window = !show_demo_window;
        }
        break;
    }
    
    // always pass input to ImGui *after*
    ImGui_ImplGlfw_KeyCallback(app->window, key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void engine::Scene::mouse_callback(double xposIn, double yposIn)
{
    if (is_editor_mode || show_demo_window)
        ImGui_ImplGlfw_CursorPosCallback(app->window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void engine::Scene::scroll_callback(double xoffset, double yoffset)
{
    (void)xoffset;   //Do nothing
    (void)yoffset;   //Do nothing

    if (is_editor_mode || show_demo_window)
        ImGui_ImplGlfw_ScrollCallback(app->window, xoffset, yoffset); // ??????????
}

// https://github.com/SonarSystems/OpenGL-Tutorials/blob/master/GLFW%20Joystick%20Input/main.cpp
void engine::Scene::gamepad_callback(const GLFWgamepadstate& state)
{
    getActiveCamera()->processJoystickMovement(state);

    //std::cout << "Left Stick X Axis: " << state.axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Left Stick Y Axis: " << state.axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Stick X Axis: " << state.axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Stick Y Axis: " << state.axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Left Trigger/L2: " << state.axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Trigger/R2: " << state.axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable

    if (GLFW_PRESS == state.buttons[1])
    {
        std::cout << "Pressed" << std::endl;
    }
    else if (GLFW_RELEASE == state.buttons[0])
    {
        //std::cout << "Released" << std::endl;
    }
}

void engine::Scene::window_refresh_callback()
{
    //render();
    //glfwSwapBuffers(app->window);
    glFinish(); // important, this waits until rendering result is actually visible, thus making resizing less ugly
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void engine::Scene::framebuffer_size_callback(int newWidth, int newHeight)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, newWidth, newHeight);

    logger.info("Resize to {}/{}", newWidth, newHeight);
}

void engine::Scene::refreshFullscreen()
{
    // reinit framebuffers because width and height changed
    m_renderer->initColorFramebuffer(app->width, app->height); // TODO use MSAA version instead !!!
}

void engine::Scene::glfw_error_callback(int error, const char* description)
{
    logger.error("GLFW Error {}: {}\n", error, description);
    std::exit(EXIT_FAILURE);
}

void engine::Scene::initQueries()
{
    glGenQueries(2, m_primitiveQuery);   // double-buffered primitive count
    glGenQueries(2, m_timerQuery);       // double-buffered GPU timer
}

void engine::Scene::beginQuery()
{
    glBeginQuery(GL_PRIMITIVES_GENERATED, m_primitiveQuery[m_queryFrameIndex]);
    glBeginQuery(GL_TIME_ELAPSED, m_timerQuery[m_queryFrameIndex]);
}

void engine::Scene::endQuery()
{
    int prevIndex = 1 - m_queryFrameIndex;

    // Check if primitive count result is available
    GLint primitiveAvailable = 0;
    if (!glIsQuery(m_primitiveQuery[prevIndex]) == GL_FALSE)
    {
        glGetQueryObjectiv(m_primitiveQuery[prevIndex], GL_QUERY_RESULT_AVAILABLE, &primitiveAvailable);
        if (primitiveAvailable) {
            glGetQueryObjectiv(m_primitiveQuery[prevIndex], GL_QUERY_RESULT, &polycount);
        }
    }

    // Check if timer result is available
    GLint timerAvailable = 0;
    if (!glIsQuery(m_timerQuery[prevIndex]) == GL_FALSE)
    {
        glGetQueryObjectiv(m_timerQuery[prevIndex], GL_QUERY_RESULT_AVAILABLE, &timerAvailable);
        if (timerAvailable) {
            GLuint64 elapsedTime;
            glGetQueryObjectui64v(m_timerQuery[prevIndex], GL_QUERY_RESULT, &elapsedTime);
            gpuTime = elapsedTime / 1e6; // convert to ms
        }
    }

    glEndQuery(GL_TIME_ELAPSED);
    glEndQuery(GL_PRIMITIVES_GENERATED);

    // Swap index for next frame
    m_queryFrameIndex = prevIndex;
}


void engine::Scene::cleanupQueries()
{
    glDeleteQueries(2, m_timerQuery);
    glDeleteQueries(2, m_primitiveQuery);
}


void engine::Scene::countItems(std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        for (auto& child : entity->children)
        {
            if (child)
            {
                if (auto modelComponent = child->getComponent<ModelComponent>())
                {
                    if (auto model = modelComponent->getModel())
                    {
                        meshcount += model->getMeshCount();
                        countItems(child);
					}
                }
                else if (child->getComponent<PrimitiveComponent>())
                {
                    primitivecount += 1;
                }
            }
        }
    }
}

#if EDITOR_MODE
void engine::Scene::renderGuizmo()
{
    auto cam = getActiveCamera();

    if (!cam)
        return;

    glm::mat4 projection = cam->getProjectionMatrix(app->width, app->height, 0.1f, 100.0f);
    glm::mat4 view = cam->getViewMatrix();

    // Convert glm::mat4 to const float*
    const float* projectionPtr = glm::value_ptr(projection);
    const float* viewPtr = glm::value_ptr(view);

    float* projectionPtr2 = glm::value_ptr(projection);
    float* viewPtr2 = glm::value_ptr(view);

    // Get the GLFW window position and size
    GLFWwindow* window = glfwGetCurrentContext();
    int windowX, windowY;
    glfwGetWindowPos(window, &windowX, &windowY);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);


    if (m_displayObjectTransformGuizmo)
    {
        ImGuizmo::SetOrthographic(!cam->isPerspective);
        ImGuizmo::BeginFrame();

        ImGui::SetNextWindowPos(ImVec2(windowX + windowWidth / 2.0f - 128.0f, windowY + 10.0f));
        ImGui::SetNextWindowSize(ImVec2(256, 46));

        static bool open{};
        ImGui::Begin("Editor", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (m_selectedEntity && m_selectedEntity->name != EntityManager::ROOT_ENTITY_NAME)
        {
            glm::mat4& objectMatrix = m_selectedEntity->getWorldTransform();
            float* objectMatrixPtr = glm::value_ptr(objectMatrix);

            for (int matId = 0; matId < gizmoCount; matId++)
            {
                ImGuizmo::SetID(matId);

                editTransform(viewPtr, projectionPtr2, glm::value_ptr(objectMatrix[matId]), lastUsing == matId, m_selectedEntity, windowX, windowY, windowWidth, windowHeight);
                if (ImGuizmo::IsUsing())
                {
                    lastUsing = matId;
                }
            }
        }

        ImGui::End();
    }

    if (m_displayViewTransformGuizmo)
    {
        // Calculate the guizmo position relative to the window's top-right corner
        ImVec2 pos = !app->fullscreen ? ImVec2(windowX + windowWidth - 128.0f, windowY + 0) : ImVec2(windowWidth - 128.0f, 0);
        ImVec2 size = ImVec2(128, 128);

        // box displayed in the upper right corner
        if (ImGuizmo::ViewManipulate(viewPtr2, camDistance, pos, size, 0x10101010))
        {
            // Get the updated view matrix
            glm::mat4 updatedViewMatrix = glm::make_mat4(viewPtr2);

            // Decompose the original view matrix to get its rotation and position
            glm::vec3 originalPosition, newPosition, scale;
            glm::quat originalRotation;

            // Decompose the original view matrix
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(view, scale, originalRotation, originalPosition, skew, perspective);

            // Decompose the updated view matrix to get the new position
            glm::decompose(updatedViewMatrix, scale, originalRotation, newPosition, skew, perspective);

            // Reconstruct the view matrix with the new position and the original rotation
            glm::mat4 newViewMatrix = glm::translate(glm::mat4(1.0f), newPosition) * glm::mat4_cast(originalRotation);

            // Set the new view matrix
            cam->setFromViewMatrix(newViewMatrix);
        }
    }
}

void engine::Scene::editTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition, std::shared_ptr<Entity> entity, int windowX, int windowY, int windowWidth, int windowHeight)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;


    if (editTransformDecomposition)
    {
        ImGui::BeginGroup();
        EditorHelper::addIconButton("translate", []() { mCurrentGizmoOperation = ImGuizmo::TRANSLATE; });
        ImGui::SameLine();
        EditorHelper::addIconButton("rotate", []() { mCurrentGizmoOperation = ImGuizmo::ROTATE; });
        ImGui::SameLine();
        EditorHelper::addIconButton("scale", []() { mCurrentGizmoOperation = ImGuizmo::SCALE; });
        ImGui::EndGroup();

        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
        {
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            //for (auto& [k, v] : iconToggleStates) v = false; // Turn all off
            //iconToggleStates["translate"] = true; // Turn only this one on
            EditorHelper::resetIconToggleStates();
            EditorHelper::setIconToggleState("translate", true);
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        {
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
            //for (auto& [k, v] : iconToggleStates) v = false; // Turn all off
            //iconToggleStates["rotate"] = true; // Turn only this one on
            EditorHelper::resetIconToggleStates();
            EditorHelper::setIconToggleState("rotate", true);
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        {
            mCurrentGizmoOperation = ImGuizmo::SCALE;
            //for (auto& [k, v] : iconToggleStates) v = false; // Turn all off
            //iconToggleStates["scale"] = true; // Turn only this one on
            EditorHelper::resetIconToggleStates();
            EditorHelper::setIconToggleState("scale", true);
        }
    }


    //if (editTransformDecomposition)
    //{
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S)) // r Key
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;
    //    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
    //        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
    //        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
    //        mCurrentGizmoOperation = ImGuizmo::SCALE;

    //    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    //    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
    //    ImGui::InputFloat3("Tr", matrixTranslation);
    //    ImGui::InputFloat3("Rt", matrixRotation);
    //    ImGui::InputFloat3("Sc", matrixScale);
    //    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

    //    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    //    {
    //        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
    //            mCurrentGizmoMode = ImGuizmo::LOCAL;
    //        ImGui::SameLine();
    //        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
    //            mCurrentGizmoMode = ImGuizmo::WORLD;
    //    }
    //    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_F10))
    //        useSnap = !useSnap;
    //    ImGui::Checkbox("Snap", &useSnap);
    //    ImGui::SameLine();

    //    switch (mCurrentGizmoOperation)
    //    {
    //    case ImGuizmo::TRANSLATE:
    //        ImGui::InputFloat3("Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::ROTATE:
    //        ImGui::InputFloat("Angle Snap", &snap[0]);
    //        break;
    //    case ImGuizmo::SCALE:
    //        ImGui::InputFloat("Scale Snap", &snap[0]);
    //        break;
    //    }
    //    ImGui::Checkbox("Bound Sizing", &boundSizing);
    //    if (boundSizing)
    //    {
    //        ImGui::PushID(3);
    //        ImGui::Checkbox("", &boundSizingSnap);
    //        ImGui::SameLine();
    //        ImGui::InputFloat3("Snap", boundsSnap);
    //        ImGui::PopID();
    //    }
    //}

    ImGuizmo::SetRect(windowX, windowY, windowWidth, windowHeight);
    if (ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL))
    {
        float matrixTranslation2[3], matrixRotation2[3], matrixScale2[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation2, matrixRotation2, matrixScale2);

        auto ttt = Transform{ glm::vec3(matrixTranslation2[0],matrixTranslation2[1], matrixTranslation2[2]), glm::vec3(matrixScale2[0], matrixScale2[1], matrixScale2[2]), glm::vec3(matrixRotation2[0], matrixRotation2[1], matrixRotation2[2])};
        entity->setTransform(ttt);
        entity->updateSelfAndChild();
    }
}


//void engine::Scene::addIcon(const std::string& icon, std::function<void()> onClick)
//{
//    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));
//
//    // Set default or toggled colors BEFORE rendering the button
//    if (iconToggleStates[icon]) {
//        // Toggled ON colors
//        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE400));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE700));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::ColorConvertU32ToFloat4(ImGui::Spectrum::BLUE500));
//    }
//    else {
//        // Toggled OFF colors
//        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.5f, 1.0f));
//        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.3f, 1.0f));
//    }
//
//    GLuint my_texture_id = getEditTransformIcon(icon);
//    if (ImGui::ImageButton(std::format("##{}", icon).c_str(), (ImTextureID)(intptr_t)my_texture_id, ImVec2(18, 18))) {
//        for (auto& [k, v] : iconToggleStates) v = false; // Turn all off
//        iconToggleStates[icon] = true; // Turn only this one on
//
//        if (onClick) onClick(); // Call the provided function
//    }
//
//    if (ImGui::IsItemHovered()) {
//        // Optional: Additional hover effects
//    }
//
//    ImGui::PopStyleVar(3); // Pop rounding, border, padding
//    ImGui::PopStyleColor(3); // Pop colors
//}

//GLuint engine::Scene::getEditTransformIcon(const std::string& key)
//{
//    auto it = m_iconEditorBarTextureCache.find(key);
//    if (it != m_iconEditorBarTextureCache.end())
//    {
//        return it->second;
//    }
//    else {
//        auto iconName = std::format("editor_{}.png", key);
//        GLuint iconTexture = Texture::loadGLTextureFromFile(iconName.c_str(), "icons");
//
//        m_iconEditorBarTextureCache.insert(std::make_pair(key, iconTexture));
//
//        return iconTexture;
//    }
//}

void engine::Scene::performRayCasting(double xpos, double ypos)
{
    // Get window dimensions
    float windowWidth = app->width;
    float windowHeight = app->height;

    // Convert to NDC
    float x = (2.0f * xpos) / windowWidth - 1.0f;
    float y = 1.0f - (2.0f * ypos) / windowHeight;
    glm::vec3 rayNDC = glm::vec3(x, y, -1.0f); // Use -1.0f for near plane

    auto cam = getActiveCamera();

    // Convert to eye space
    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(cam->zoom),
        (float)windowWidth / (float)windowHeight,
        0.1f, 100.0f
    );
    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
    glm::vec4 rayEye = inverseProjection * glm::vec4(rayNDC, 1.0f);
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Direction vector in eye space

    // Normalize the ray direction in eye space
    glm::vec3 rayEyeDir = glm::normalize(glm::vec3(rayEye));

    // Convert to world space
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 rayWorldDir = glm::vec3(inverseView * glm::vec4(rayEyeDir, 0.0f));
    glm::vec3 rayOrigin = cam->position; // Ray origin is the camera position in world space

    // Track closest hit
    float closestDistance = std::numeric_limits<float>::max();
    std::shared_ptr<Entity> pickedEntity = nullptr;

    // Iterate through entities
    auto entity = m_entityManager.getRootEntity();
    if (entity)
    {
        for (auto& child : entity->children)
        {
            if (child)
            {
                if (auto modelComponent = child->getComponent<ModelComponent>())
                {
                    AABB* vol = modelComponent->getBoundingVolume();
                    if (vol)
                    {
                        float distance;
                        if (testRayAABBIntersection(rayOrigin, rayWorldDir, vol, distance))
                        {
                            if (distance < closestDistance)
                            {
                                closestDistance = distance;
                                pickedEntity = child;
                            }
                        }
                    }
                }
                else if (auto primitiveComponent = child->getComponent<PrimitiveComponent>())
                {
                    AABB* vol = primitiveComponent->getBoundingVolume();
                    if (vol)
                    {
                        float distance;
                        if (testRayAABBIntersection(rayOrigin, rayWorldDir, vol, distance))
                        {
                            if (distance < closestDistance)
                            {
                                closestDistance = distance;
                                pickedEntity = child;
                            }
                        }
                    }
                }
            }
        }
    }

    // Highlight the closest picked entity
    if (pickedEntity)
    {
        if (auto modelComponent = pickedEntity->getComponent<ModelComponent>())
        {
            modelComponent->getModel()->highlight = true;
            std::cout << "Picked model: " << pickedEntity->name << std::endl;
        }
        else if (auto primitiveComponent = pickedEntity->getComponent<PrimitiveComponent>())
        {
            primitiveComponent->getPrimitive()->highlight = true;
            std::cout << "Picked primitive: " << pickedEntity->name << std::endl;
        }
    }
}


//void engine::Scene::performRayCasting(double xpos, double ypos)
//{
//    // Get window dimensions (you may need to pass these or store them)
//    float windowWidth = app->width;  // Replace with actual width
//    float windowHeight = app->height; // Replace with actual height
//
//    // Convert to NDC
//    float x = (2.0f * xpos) / windowWidth - 1.0f;
//    float y = 1.0f - (2.0f * ypos) / windowHeight;
//    //glm::vec3 rayNDC = glm::vec3(x, y, 1.0f);
//    glm::vec3 rayNDC = glm::vec3(x, y, -1.0f); // Use -1.0f for near plane
//
//    auto cam = getActiveCamera();
//    
//    // Convert to eye space
//    glm::mat4 projectionMatrix = glm::perspective(glm::radians(cam->zoom), (float)app->width / (float)app->height, 0.1f, 100.0f);
//    glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
//    glm::vec4 rayEye = inverseProjection * glm::vec4(rayNDC, 1.0f);
//    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Direction vector in eye space
//
//    // Normalize the ray direction in eye space
//    glm::vec3 rayEyeDir = glm::normalize(glm::vec3(rayEye));
//
//    // Convert to world space
//    glm::mat4 viewMatrix = cam->getViewMatrix();
//    glm::mat4 inverseView = glm::inverse(viewMatrix);
//    glm::vec3 rayWorldDir = glm::vec3(inverseView * glm::vec4(rayEyeDir, 0.0f));
//    glm::vec3 rayOrigin = cam->position; // Ray origin is the camera position in world space
//
//
//    // Convert to eye space
//    //glm::mat4 projectionMatrix = glm::perspective(glm::radians(cam->zoom), (float)app->width / (float)app->height, 0.1f, 100.0f);
//    //glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
//    //glm::vec4 rayEye = inverseProjection * glm::vec4(rayNDC, 1.0f);
//    //rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
//
//    //// Convert to world space
//    //glm::mat4 viewMatrix = cam->getViewMatrix();
//    //glm::mat4 inverseView = glm::inverse(viewMatrix);
//    //glm::vec3 rayWorld = glm::vec3(inverseView * rayEye);
//    //glm::vec3 rayDirection = glm::normalize(rayWorld);
//
//    // Ray origin is the camera position
//    //glm::vec3 rayOrigin = cam->position;
//
//    // Rest of your code for ray-AABB intersection
//    auto entity = m_entityManager.getRootEntity();
//    if (entity)
//    {
//        for (auto& child : entity->children)
//        {
//            if (child)
//            {
//                if (auto modelComponent = child->getComponent<ModelComponent>())
//                {
//                    AABB* vol = modelComponent->getBoundingVolume();
//                    if (vol)
//                    {
//                        if (testRayAABBIntersection(rayOrigin, rayDirection, vol)) {
//                            modelComponent->getModel()->highlight = true;
//                            std::cout << "picked model " << child->name << std::endl;
//                            break;
//                        }
//                    }
//                }
//                else if (auto primitiveComponent = child->getComponent<PrimitiveComponent>())
//                {
//                    AABB* vol = primitiveComponent->getBoundingVolume();
//                    if (vol)
//                    {
//                        if (testRayAABBIntersection(rayOrigin, rayDirection, vol)) {
//                            primitiveComponent->getPrimitive()->highlight = true;
//                            std::cout << "picked primitive " << child->name << std::endl;
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

bool engine::Scene::testRayAABBIntersection(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDirection,
    const engine::AABB* aabb,
    float& outDistance)
{
    glm::vec3 boxMin = aabb->center - aabb->extents;
    glm::vec3 boxMax = aabb->center + aabb->extents;

    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();

    for (int i = 0; i < 3; i++)
    {
        if (std::abs(rayDirection[i]) < 1e-6f)
        {
            // Ray is parallel to the slab
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                return false;
        }
        else
        {
            float t1 = (boxMin[i] - rayOrigin[i]) / rayDirection[i];
            float t2 = (boxMax[i] - rayOrigin[i]) / rayDirection[i];
            tmin = std::max(tmin, std::min(t1, t2));
            tmax = std::min(tmax, std::max(t1, t2));
        }
    }

    outDistance = tmin;
    return tmax >= tmin;
}

#endif