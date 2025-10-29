#include "../../include/app/scene.h"

#include "extensions/imoguizmo.hpp"
#include "../../include/managers/log_manager.h"
#include "../../include/singleton.h"

#include "../../include/debug/opengl_debug.h"

#include <glm/gtc/type_ptr.hpp>


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

    // renderer setup
    m_renderer->setup(app->width, app->height, getActiveCamera(), lights);

    // listen for editor selected entity changed
    #if EDITOR_MODE
    listenForEditorChanges();
    #endif

    after_init();
}


void engine::Scene::computeLightsIndexes()
{
    unsigned int nextPointLightIndex = 0;
    unsigned int nextDirLightIndex = 0;
    unsigned int nextSpotLightIndex = 0;
    unsigned int nextAreaLightIndex = 0;

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
            m_selectedEntityID = entity->id;
        });

    m_editor.setOnSceneSettingChanged([this](std::string key, bool value)
        {
            logger.info("{} setting changed: {})", key, value);

            auto* singleton = engine::Singleton::getInstance();
            assert(singleton != nullptr && "Singleton not initialized !");
            SceneSettings& sceneSettings = singleton->sceneSettings();

            if (key == "draw_wireframe") {
                sceneSettings.drawAsWireframe = value;
            }
            else if (key == "enable_gamma_correction") {
                sceneSettings.enableGammaCorrection = value;
            }
            else if (key == "enable_face_culling") {
                sceneSettings.enableFaceCulling = value;
            }
            else if (key == "enable_camera_frustrum_culling") {
                sceneSettings.enableCameraFrustrumCulling = value;
			}
            else if (key == "draw_lights_visual_helpers") {
                sceneSettings.drawLightsVisualHelpers = value;
            }
            else if (key == "draw_bounding_boxes_visual_helpers") {
                sceneSettings.drawBoundingBoxesVisualHelpers = value;
            }
            else if (key == "draw_debug_normals_visual_helpers") {
                sceneSettings.drawNormalsVisualHelpers = value;
            }
        });
}
#endif

void engine::Scene::gameLoop()
{
    OpenGLDebug::GLClearError();
    
    
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
        app->setWindowTitle("EDITOR");
        m_editor.renderUIWindow(is_editor_mode);
        renderGizmo();
    }
    #endif
    
    if (show_perf_overlay)
        m_perfOverlay.renderPerfOverlay(&show_perf_overlay, framerate, cpuTime, gpuTime, uiTime);

    // Dear ImGui demo windows
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    


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

    OpenGLDebug::GLCheckError();
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
    glm::mat4 projection = glm::perspective(glm::radians(cam->zoom), (float)app->width / (float)app->height, 0.1f, 100.0f);
    glm::mat4 view = cam->getViewMatrix();
    const Frustum camFrustum = cam->createFrustumFromCamera((float)app->width / (float)app->height, glm::radians(cam->zoom), 0.1f, 100.0f);


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
        else
        {
            // Only draw outline where stencil != objectID
            glStencilFunc(GL_NOTEQUAL, entity->id, 0xFF);
            glStencilMask(0x00); // disable stencil writes

            shader.setMat4("view", view);
            shader.setMat4("projection", projection);
            shader.setFloat("outlineWidth", entity->id == m_selectedEntityID ? 0.08f : 0.0f);
        }

        auto transform = entity->getTransform();

        // looping over entity components
        for (const auto& [typeID, component] : entity->components)
        {
            if (typeID == ComponentType::primitive || typeID == ComponentType::model)
            {
                // primitive and model
                component->draw(projection, view, shader, entity->getWorldTransform(), transform, entity->getBoundingVolume());
                inFrustrumCount++;
            }
            else if (typeID == ComponentType::light)
            {
                // light
                component->draw(projection, view, shader, entity->getWorldTransform(), transform);
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
    case GLFW_KEY_W:
        if (action == GLFW_PRESS && !key_w_pressed)
        {
            is_editor_mode = !is_editor_mode;
            key_w_pressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            key_w_pressed = false;
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
    m_renderer->initColorFramebuffer(app->width, app->height);
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
                        meshcount += model->getNumberOfMeshes();
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

void engine::Scene::renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
             // bottom face
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
              1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
              1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
             -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             // top face
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
              1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
              1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
              1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    // Send to GPU
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


void engine::Scene::renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // send to GPU
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void engine::Scene::renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = static_cast<GLsizei>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        
        // setup sphere VAO
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    // Send to GPU
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#if EDITOR_MODE
void engine::Scene::renderGizmo()
{
    // it is recommended to use a separate projection matrix since the values that work best
    // can be very different from what works well with normal renderings
    // e.g., with glm -> glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);

    // optional: configure color, axis length and more
    ImOGuizmo::config.axisLengthScale = 0.33f;
    ImOGuizmo::config.lineThicknessScale = 0.027f;

    ImVec2 avail = ImGui::GetContentRegionAvail();

    // Position gizmo in top-right corner of this window
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImOGuizmo::SetRect(
        windowPos.x + avail.x + 440.0f, // stick to top-right
        windowPos.y - 50.0f,
        100.0f
    );


    ImOGuizmo::BeginFrame();

    glm::mat4 projMat = glm::perspective(glm::radians(getActiveCamera()->zoom), static_cast<float>(app->width) / static_cast<float>(app->height), 0.1f, 100.0f);
    const float* projPtr = glm::value_ptr(projMat);

    glm::mat4 viewMatrix = getActiveCamera()->getViewMatrix();
    float* viewPtr = glm::value_ptr(viewMatrix);

    float pivotDistance = 1.0f;
    // optional: set distance to pivot (-> activates interaction)
    if (ImOGuizmo::DrawGizmo(viewPtr, projPtr, pivotDistance))
    {
        // in case of user interaction viewMatrix gets updated
        // Apply snapped view back to camera
        getActiveCamera()->setFromViewMatrix(viewMatrix);
    }
}
#endif