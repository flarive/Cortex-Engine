#include "../../include/app/scene.h"

#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"

#include "../../include/debug/opengl_debug.h"
#include "../../include/debug/debug_frame.h"

#include "../../include/tools/helpers.h"
#include "../../include/aabb.h"

#include "../../include/lights/light.h"

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
#include <cstdint>


uint64_t globalFrameIndex = 0;



namespace engine {
    Scene* Scene::currentInstance = nullptr; // Definition
}

using Clock = std::chrono::high_resolution_clock;



engine::Scene::Scene(const std::string& _title, std::weak_ptr<App> _app, SceneSettings _settings)
    : title(_title), m_app(_app), m_sceneSettings(_settings)
{
    logger.trace("Scene {} base constructor called", title);
}

engine::Renderer* engine::Scene::getRenderer() const
{
    // no ownership transfer, read-only access
    return m_renderer.get();
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

    // Count all lights in the scene
    computeLightCount();

    // Fill imGui debug window with current scene hierarchy
    #if EDITOR_MODE
    m_editor.init();
    m_editor.setScene(m_entityManager.getRootEntity());
    m_editor.initEditor();
    #endif
    
    // Count all items in the scene
    countItems(m_entityManager.getRootEntity());

    initQueries();

    // Avoid a big deltatime (nearly 3s) the first time
    lastFrameTime = (float)glfwGetTime();
}

void engine::Scene::initialize()
{
    m_isInitialized = false;

    // Convert weak_ptr to shared_ptr
    auto appShared = m_app.lock();
    if (!appShared) {
        throw std::runtime_error("App no longer exists!");
    }

    // Create renderer
    if (m_sceneSettings.method == RenderMethod::PBR) {
        // PBR is default renderer (mainstream)
        m_renderer = std::make_unique<PbrRenderer>(appShared->window);
    }
    else if (m_sceneSettings.method == RenderMethod::BlinnPhong) {
        // BlinnPhong is a legacy renderer (deprecated)
        m_renderer = std::make_unique<BlinnPhongRenderer>(appShared->window);
    }
    else {
        // Blinn is just for very simple tests
        m_renderer = std::make_unique<PhongRenderer>(appShared->window);
    }

    // create scene entities hierarchy
    m_entityManager.create();

    // store SceneSettings in a singleton for easy access everywhere
    engine::Singleton::initialize(m_sceneSettings);

    currentInstance = this; // Set the current instance when constructing
    
    before_init();

    init();

    lights = m_entityManager.findEntitiesOfType<Light>();
	if (lights.size() == 0) logger.warn("Scene has no light !");

    // assign light indexes
    computeLightsIndexes();

    cameras = m_entityManager.findEntitiesOfType<Camera>();
    if (cameras.size() == 0) logger.warn("Scene has no camera !");

    m_editor.initRenderGuizmo(getActiveCamera());

    // renderer should use tessellation shaders ?
    computeSupportTessellation(m_entityManager.getRootEntity());
    m_renderer->setShouldSupportTessellation(m_supportTessellation);

    if (auto appPtr = getApp()) {
        // renderer setup
        m_renderer->setup(static_cast<int>(appPtr->width), static_cast<int>(appPtr->height), getActiveCamera(), lights);
    }
    else {
        // Handle the case where App no longer exists
        throw std::runtime_error("App no longer exists!");
        // Or log an error, or return early, depending on your needs
    }
    

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

    m_isInitialized = true;
}

// Callback function
//void engine::Scene::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//{
//    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//        // Get mouse position and perform ray casting
//        double xpos, ypos;
//        glfwGetCursorPos(window, &xpos, &ypos);
//
//        if (currentInstance)
//        {
//			currentInstance->performRayCasting(xpos, ypos);
//        }
//    }
//}



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
            if (!m_selectedEntity || (m_selectedEntity->id != entity->id))
            {
                logger.info("Selected entity changed: {} (id {})", entity->name, entity->id);
                m_selectedEntity = entity;
            }
        });

    m_editor.setOnSceneSettingChanged([this](const std::string& key, SceneSetting value)
        {
            auto* singleton = engine::Singleton::getInstance();
            assert(singleton != nullptr && "Singleton not initialized !");
            SceneSettings& sceneSettings = singleton->sceneSettings();

			bool boolValue = false;
			int intValue = 0;
            uint unsignedIntValue = 0;
            ubyte unsignedByteValue = 0;
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
            else if (std::holds_alternative<uint>(value))
            {
                unsignedIntValue = std::get<uint>(value);
                logger.info("{} setting changed: {}", key, unsignedIntValue);
            }
            else if (std::holds_alternative<ubyte>(value))
            {
                unsignedByteValue = std::get<ubyte>(value);
                logger.info("{} setting changed: {}", key, unsignedByteValue);
            }

            if (key == "render_method") {
                sceneSettings.method = static_cast<RenderMethod>(intValue);
            }
            else if (key == "draw_wireframe") {
                sceneSettings.drawAsWireframe = boolValue;
            }
            else if (key == "enable_gamma_correction") {
                sceneSettings.enableGammaCorrection = boolValue;
            }
            else if (key == "enable_tone_mapping") {
                sceneSettings.enableToneMapping = boolValue;
            }
            else if (key == "post_process") {
                sceneSettings.applyPostProcessFx = static_cast<PostProcessingEffect>(intValue);
            }
            else if (key == "exposure") {
                sceneSettings.exposure = floatValue;
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
                sceneSettings.shadowCalculationMethod = static_cast<ShadowCalculationMethod>(intValue);
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
            else if (key == "pbr_ibl_diffuse_intensity") {
                sceneSettings.iblDiffuseIntensity = floatValue;
            }
            else if (key == "pbr_ibl_specular_intensity") {
                sceneSettings.iblSpecularIntensity = floatValue;
            }
            else if (key == "framebuffer_msaa_samples") {
                sceneSettings.frameBufferAntiAliasingSamplesQuality = unsignedByteValue;
            }
        });
}
#endif

void engine::Scene::gameLoop()
{
    if (!m_isInitialized)
        assert(m_isInitialized && "Scene not initialized");
    
    // Start CPU timer
    auto cpuFrameStart = Clock::now();


    auto app = getApp();
    if (!app) return;

    auto& sceneManager = app->getSceneManager();

    if (sceneManager.shouldUnloadScene())
    {
        sceneManager.unloadCurrentScene();
    }

    ImGuiIO& io = ImGui::GetIO();

    if (glfwGetWindowAttrib(app->window, GLFW_ICONIFIED) != 0)
    {
        // Even if minimized, update ImGui deltaTime to avoid huge spikes
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
        io.DeltaTime = deltaTime;

        ImGui_ImplGlfw_Sleep(10);
        return;
    }

    globalFrameIndex++; // once per frame

    
    
    float currentFrameTime = static_cast<float>(glfwGetTime());
    deltaTime = currentFrameTime - lastFrameTime; // Should be arround 0.016–0.020s (60–50 fps)
    lastFrameTime = currentFrameTime;
    io.DeltaTime = deltaTime;


    // measure ui time (part 1 begin)
    auto uiStart1 = Clock::now();

    const float width = app->width;
    const float height = app->height;
	const bool capFramerate = app->capFramerate();
    const int frameDelay = app->getFrameDelay();
    GLFWwindow* window = app->window; // better as const ?
    

    auto camera = getActiveCamera();
    if (!camera)
        return;


    float aspect = (height > 0.0f) ? (width / height) : 1.0f;
    glm::mat4 projection = camera->getProjectionMatrix(aspect);
    glm::mat4 view = camera->getViewMatrix();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    framerate = io.Framerate;

    // Switch to Editor mode if needed
    setEditorMode(projection, view);
    

    // Dear ImGui demo windows
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    // measure ui time (part 1 end)
    auto uiEnd1 = Clock::now();
    std::chrono::duration<double, std::milli> uiDuration1 = uiEnd1 - uiStart1;
    uiTime = uiDuration1.count();


    if (show_perf_overlay && !is_editor_mode)
    {
        m_perfOverlay.init();
        m_perfOverlay.updatePerformanceCounters({ framerate, deltaTime, cpuTime, gpuTime, uiTime });
        m_perfOverlay.render();
    }

    // fps capping (begin)
    std::chrono::steady_clock::time_point start_time{};
    if (capFramerate) {
        start_time = Clock::now();
    }

    // get opengl stats such as polycount drawn, GPU timer...
    beginQuery();

    // Lambda to update
    auto updateLambda = [this](Shader& shader, Shader& shaderTessellation) {
        // execute scene update
        if (shader.name != "outline")
        {
            update(shader);
            update(shaderTessellation);
        }

        // draw our scene graph
        // called 2 times, one for renderer and another one for shadow maps
        drawEntities(shader, shaderTessellation);
        };

    // Lambda to update the UI
    auto updateUILambda = [this]() {
        updateUI();
        };

    // Call the renderer loop
    m_renderer->loop(static_cast<int>(width), static_cast<int>(height), camera, updateLambda, updateUILambda);

    // get opengl stats such as polycount drawn, GPU timer...
    endQuery();

    // measure ui time (part 2 begin)
    auto uiStart2 = Clock::now();

    // ImGUI rendering
    ImGui::Render();


    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    // For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    
    if (sceneManager.shouldUnloadScene())
    {
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    glfwSwapBuffers(window);
    

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    // measure ui time (part 2 end)
    auto uiEnd2 = Clock::now();
    std::chrono::duration<double, std::milli> uiDuration2 = uiEnd2 - uiStart2;
    uiTime += uiDuration2.count();

    // fps capping (end)
    if (capFramerate)
    {
        auto end_time = Clock::now();
        /*auto zzz = std::chrono::milliseconds(frameDelay) - (end_time - start_time);
        std::this_thread::sleep_for(zzz);*/

        auto frameTime = end_time - start_time;
        auto target = std::chrono::milliseconds(frameDelay);

        if (frameTime < target)
        {
            std::this_thread::sleep_for(target - frameTime);
        }
    }

    // End CPU timer
    auto cpuFrameEnd = Clock::now();
    std::chrono::duration<double, std::milli> cpuFrameDuration = cpuFrameEnd - cpuFrameStart;
    cpuTime = cpuFrameDuration.count();
}

void engine::Scene::setEditorMode(glm::mat4& projection, glm::mat4& view)
{
    m_displayObjectTransformGuizmo = is_editor_mode;
    m_displayViewTransformGuizmo = !is_editor_mode;

    #if EDITOR_MODE
    if (auto appPtr = getApp())
    {
        if (is_editor_mode)
        {
            appPtr->setWindowTitleSuffix("[EDITOR]");
            m_editor.renderEditor(is_editor_mode, projection, view, m_displayObjectTransformGuizmo);
        }
        else
        {
            appPtr->resetWindowTitleSuffix();
        }

        appPtr->setWindowTitle();
    }
    #endif

    // render camera view guizmo in the top right corner of the screen
    m_editor.renderViewGuizmo(projection, view, m_displayViewTransformGuizmo);
}

void engine::Scene::initEntities()
{
    // Init using stored world transforms
    initEntityRecursive(m_entityManager.getRootEntity());
}

void engine::Scene::initEntityRecursive(const std::shared_ptr<engine::Entity>& entity)
{
    for (const auto& [typeID, component] : entity->components)
    {
        if (typeID != ComponentType::transform)
        {
            component->init(entity->getTransform());
        }
    }

    // init children
    for (const auto& child : entity->children)
    {
        initEntityRecursive(child);
    }
}

void engine::Scene::drawEntities(Shader& shader, Shader& shaderTessellation)
{
    //DebugFrame::ensureIsCalledOncePerFrame("Scene", "drawEntities");
    
    inFrustrumCount = 0;
    totalFrustrumCount = 0;
    
    // draw flat and nested entity hierarchy
    // Precompute transforms for all entities before drawing
    if (shader.name != "outline")
        m_entityManager.getRootEntity()->updateSelfAndChild();

    float width = 0;
    float height = 0;


    if (auto appPtr = getApp()) {
        width = appPtr->width;
        height = appPtr->height;
    }

    auto cam = getActiveCamera();
    float ratio = width / height;
    glm::mat4 projection = cam->getProjectionMatrix(ratio);
    glm::mat4 view = cam->getViewMatrix();
    const Frustum camFrustum = cam->createFrustumFromCamera(ratio, glm::radians(cam->getZoom()), 0.1f, 100.0f);

    
    // check the number of times entities are drawn per frame (should be called only once !)
    extern uint64_t globalFrameIndex;
    static uint64_t lastFrameSeen = UINT64_MAX;
    static int callsThisFrame = 0;

    if (globalFrameIndex != lastFrameSeen)
    {
        callsThisFrame = 0;
        lastFrameSeen = globalFrameIndex;
    }

    callsThisFrame++;


    // Draw using stored world transforms
    drawEntityRecursive(m_entityManager.getRootEntity(), shader, shaderTessellation, projection, view, camFrustum, callsThisFrame);
}

void engine::Scene::drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader, Shader& shaderTessellation, const glm::mat4& projection, const glm::mat4& view, const Frustum& camFrustum, const int& callsThisFrame)
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

        auto& transform = entity->getTransform();

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
                        auto& canCastShadows = properties.at("canCastShadows");
                        if (auto pBool = std::get_if<bool>(&canCastShadows.value))
                        {
                            shouldDraw = *pBool;
                        }
                    }
                }
                
                // primitive and model
                if (shouldDraw) {
                    component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());
                }

                inFrustrumCount++;
            }
            else if (typeID == ComponentType::light)
            {
                component->draw(projection, view, shader, entity->getWorldTransform(), transform);

                // TODO !!!! test if tesselation is used/needed
                component->draw(projection, view, shaderTessellation, entity->getWorldTransform(), transform);
            }
            else if (typeID == ComponentType::animator)
            {
                // update should be called only one time per frame
                if (callsThisFrame == 1)
                    component->update(deltaTime, transform);

                component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());
            }
            else if (typeID == ComponentType::particleSystem)
            {
                // update should be called only one time per frame
                if (callsThisFrame == 1)
                    component->update(deltaTime, transform);
                
                component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());
                inFrustrumCount++;
            }
            else if (typeID == ComponentType::terrain)
            {
                // update should be called only one time per frame
                if (callsThisFrame == 1)
                    component->update(deltaTime, transform);

                component->draw(projection, view, shaderTessellation, entity->getWorldTransform(), transform, entity->getBoundingVolume());
                inFrustrumCount++;
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
            drawEntityRecursive(child, shader, shaderTessellation, projection, view, camFrustum, callsThisFrame);
        }
    }

    auto entityType = entity->getType(); // could be optimized/avoided
    if (entityType != EntityType::undefined && entityType != EntityType::light && entityType != EntityType::camera)
    {
        totalFrustrumCount++;
    }
}

void engine::Scene::exit()
{
    logger.info("Exiting scene {}", title);
    
    // Optional: Unbind OpenGL state (if context is still active)
    glBindVertexArray(0);

    // User-defined cleanup (e.g., saving scene state)
    clean();

    // Note: No need to call cleanupQueries() or m_renderer->clean() here
    // if they are already in the destructor.
}

GLFWwindow* engine::Scene::getWindow()
{
    if (auto appPtr = getApp()) {
        return appPtr->window;
    }
    return nullptr;
}

void engine::Scene::key_callback(int key, int scancode, int action, int mods)
{
    (void)scancode;   //Do nothing
    (void)mods;   //Do nothing

    // basic window handling
    if (auto appPtr = getApp())
    {
        switch (key)
        {
        case GLFW_KEY_F:
            if (action == GLFW_RELEASE)
            {
                appPtr->toggleFullscreen([this]() {
                    this->refreshFullscreen();
                    });
            }
            break;
        case GLFW_KEY_F1:
            if (action == GLFW_PRESS && !key_F1_pressed) {
                is_editor_mode = !is_editor_mode;
                key_F1_pressed = true;
            }
            else if (action == GLFW_RELEASE) {
                key_F1_pressed = false;
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE) {
                show_demo_window = !show_demo_window;
            }
            break;
        }

        // always pass input to ImGui *after*
        ImGui_ImplGlfw_KeyCallback(appPtr->window, key, scancode, action, mods);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void engine::Scene::mouse_callback(double xposIn, double yposIn)
{
    if (auto appPtr = getApp(); appPtr && (is_editor_mode || show_demo_window))
        ImGui_ImplGlfw_CursorPosCallback(appPtr->window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void engine::Scene::scroll_callback(double xoffset, double yoffset)
{
    if (auto appPtr = getApp(); appPtr && (is_editor_mode || show_demo_window))
        ImGui_ImplGlfw_ScrollCallback(appPtr->window, xoffset, yoffset);
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
    if (auto appPtr = getApp())
        m_renderer->initColorFramebuffer(static_cast<int>(appPtr->width), static_cast<int>(appPtr->height)); // TODO use MSAA version instead !!!
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

void engine::Scene::beginQuery() const
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


void engine::Scene::cleanupQueries() const
{
    glDeleteQueries(2, m_timerQuery);
    glDeleteQueries(2, m_primitiveQuery);
}

uint64_t engine::Scene::getTotalElapsedFrames() const
{
    return globalFrameIndex;
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
                else if (child->getComponent<PrimitiveComponent>() || child->getComponent<ParticleSystemComponent>() || child->getComponent<TerrainComponent>())
                {
                    primitivecount += 1;
                }
            }
        }
    }
}

void engine::Scene::computeLightCount()
{
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
}

void engine::Scene::computeSupportTessellation(std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        for (auto& child : entity->children)
        {
            if (child)
            {
                // should use primitive isTessellated instead
                if (child->getComponent<TerrainComponent>())
                {
                    m_supportTessellation = true;
                    break;
                }

                if (!child->children.empty())
                    computeSupportTessellation(child);
            }
        }
    }
}

//void engine::Scene::performRayCasting(double xpos, double ypos)
//{
//    // Get window dimensions
//    float windowWidth = app->width;
//    float windowHeight = app->height;
//
//    // Convert to NDC
//    float x = (2.0f * xpos) / windowWidth - 1.0f;
//    float y = 1.0f - (2.0f * ypos) / windowHeight;
//    glm::vec3 rayNDC = glm::vec3(x, y, -1.0f); // Use -1.0f for near plane
//
//    auto cam = getActiveCamera();
//
//    // Convert to eye space
//    glm::mat4 projectionMatrix = glm::perspective(
//        glm::radians(cam->zoom),
//        (float)windowWidth / (float)windowHeight,
//        0.1f, 100.0f
//    );
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
//    // Track closest hit
//    float closestDistance = std::numeric_limits<float>::max();
//    std::shared_ptr<Entity> pickedEntity = nullptr;
//
//    // Iterate through entities
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
//                        float distance;
//                        if (testRayAABBIntersection(rayOrigin, rayWorldDir, vol, distance))
//                        {
//                            if (distance < closestDistance)
//                            {
//                                closestDistance = distance;
//                                pickedEntity = child;
//                            }
//                        }
//                    }
//                }
//                else if (auto primitiveComponent = child->getComponent<PrimitiveComponent>())
//                {
//                    AABB* vol = primitiveComponent->getBoundingVolume();
//                    if (vol)
//                    {
//                        float distance;
//                        if (testRayAABBIntersection(rayOrigin, rayWorldDir, vol, distance))
//                        {
//                            if (distance < closestDistance)
//                            {
//                                closestDistance = distance;
//                                pickedEntity = child;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    // Highlight the closest picked entity
//    if (pickedEntity)
//    {
//        if (auto modelComponent = pickedEntity->getComponent<ModelComponent>())
//        {
//            modelComponent->getModel()->highlight = true;
//            std::cout << "Picked model: " << pickedEntity->name << std::endl;
//        }
//        else if (auto primitiveComponent = pickedEntity->getComponent<PrimitiveComponent>())
//        {
//            primitiveComponent->getPrimitive()->highlight = true;
//            std::cout << "Picked primitive: " << pickedEntity->name << std::endl;
//        }
//    }
//}


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

//bool engine::Scene::testRayAABBIntersection(
//    const glm::vec3& rayOrigin,
//    const glm::vec3& rayDirection,
//    const engine::AABB* aabb,
//    float& outDistance)
//{
//    glm::vec3 boxMin = aabb->center - aabb->extents;
//    glm::vec3 boxMax = aabb->center + aabb->extents;
//
//    float tmin = -std::numeric_limits<float>::infinity();
//    float tmax = std::numeric_limits<float>::infinity();
//
//    for (int i = 0; i < 3; i++)
//    {
//        if (std::abs(rayDirection[i]) < 1e-6f)
//        {
//            // Ray is parallel to the slab
//            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
//                return false;
//        }
//        else
//        {
//            float t1 = (boxMin[i] - rayOrigin[i]) / rayDirection[i];
//            float t2 = (boxMax[i] - rayOrigin[i]) / rayDirection[i];
//            tmin = std::max(tmin, std::min(t1, t2));
//            tmax = std::min(tmax, std::max(t1, t2));
//        }
//    }
//
//    outDistance = tmin;
//    return tmax >= tmin;
//}

//#endif


engine::Scene::~Scene()
{
    logger.trace("Scene {} base destructor called", title);

    // OpenGL resources
    cleanupQueries();

    // Managed objects
    if (m_renderer) {
        m_renderer->clean();
    }
    m_entityManager.clean();
    m_audioManager.clean();

    // Reset static state
    currentInstance = nullptr;
}


