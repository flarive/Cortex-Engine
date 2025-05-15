#include "../../include/app/scene.h"


engine::Scene::Scene(std::string _title, App* _app, SceneSettings _settings)
    : title(_title), app(_app), settings(_settings)
{
    if (settings.method == RenderMethod::PBR)
    {
        m_renderer = new PbrRenderer(app->window, settings);
    }
    else
    {
        m_renderer = new BlinnPhongRenderer(app->window, settings);
    }

    // create scene entities hierarchy
    rootEntity = std::make_shared<engine::Entity>("Root");
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
    
    // Counters
    unsigned short spotLightCount = 0, dirLightCount = 0, pointLightCount = 0;

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
    }

    m_renderer->setLightsCount(pointLightCount, dirLightCount, spotLightCount);

    // Fill imGui debug window with current scene hierarchy
    m_debug.setScene(this->rootEntity);

    // count all meshes in the scene
    countMeshes(this->rootEntity);
}

void engine::Scene::initialize()
{
    before_init();

    init();

    // renderer setup
    m_renderer->setup(app->width, app->height, std::make_shared<Camera>(camera), lights);

    after_init();
}

void engine::Scene::gameLoop()
{
    // Poll and handle events (inputs, window resize, etc.)
    glfwPollEvents();

    if (glfwGetWindowAttrib(app->window, GLFW_ICONIFIED) != 0)
    {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    framerate = ImGui::GetIO().Framerate;

    if (show_window)
    {
        m_debug.renderUIWindow(show_window);
    }

    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;



    auto start_time = std::chrono::high_resolution_clock::now();


    // get opengl stats such as polycount drawn
    beginQuery();



    




    // Lambda to update
    auto updateLambda = [this](Shader& shader) {
        update(shader);

        // draw our scene graph
        drawEntities(shader);
        };

    // Lambda to update the UI
    auto updateUILambda = [this]() {
        updateUI();
        };


    // Call the method
    m_renderer->loop(app->width, app->height, std::make_shared<Camera>(camera), updateLambda, updateUILambda);

    




    // get opengl stats such as polycount drawn
    endQuery();

    // ImGUI rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(app->window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


    glfwSwapBuffers(app->window);

    // Poll and handle events (inputs, window resize, etc.)
    //glfwPollEvents();

    auto end_time = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(app->getFrameDelay()) - (end_time - start_time));
}

//void engine::Scene::drawEntities(Shader& shader)
//{
//    // draw our scene graph
//    std::shared_ptr<engine::Entity> lastEntity = rootEntity;
//    while (lastEntity && lastEntity->children.size() > 0)
//    {
//        shader.setMat4("model", lastEntity->transform.getModelMatrix());
//        if (lastEntity->model)
//        {
//            lastEntity->model->draw(shader, lastEntity->transform.getLocalPosition(), lastEntity->transform.getLocalScale(), lastEntity->transform.getLocalRotation());
//        }
//
//        lastEntity = lastEntity->children.back();
//    }
//
//    rootEntity->updateSelfAndChild();
//}

void engine::Scene::drawEntities(Shader& shader)
{
    drawEntityRecursive(rootEntity, shader);
    rootEntity->updateSelfAndChild();
}

void engine::Scene::drawEntityRecursive(const std::shared_ptr<engine::Entity>& entity, Shader& shader)
{
    // Set model matrix for current entity
    shader.setMat4("model", entity->transform.getModelMatrix());

    // Draw the entity if it has a model
    if (entity->model)
    {
        entity->model->draw(shader,
            entity->transform.getLocalPosition(),
            entity->transform.getLocalScale(),
            entity->transform.getLocalRotation());
    }

    // Recurse on children
    for (const auto& child : entity->children)
    {
        drawEntityRecursive(child, shader);
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

    // clean user stuffs
    clean();
}

GLFWwindow* engine::Scene::getWindow()
{
    return app->window;
}


void engine::Scene::key_callback(int key, int scancode, int action, int mods)
{
    UNREFERENCED_PARAMETER(scancode);
    UNREFERENCED_PARAMETER(mods);

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
    }

    if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (!key_w_pressed) // Only toggle when the key is first pressed
        {
            show_window = !show_window;
            key_w_pressed = true; // Mark the key as pressed
        }
    }
    else if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        key_w_pressed = false; // Reset the state when the key is released
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void engine::Scene::mouse_callback(double xposIn, double yposIn)
{
    if (show_window)
        ImGui_ImplGlfw_CursorPosCallback(app->window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void engine::Scene::scroll_callback(double xoffset, double yoffset)
{
    UNREFERENCED_PARAMETER(xoffset);
    UNREFERENCED_PARAMETER(yoffset);
}

// https://github.com/SonarSystems/OpenGL-Tutorials/blob/master/GLFW%20Joystick%20Input/main.cpp
void engine::Scene::gamepad_callback(const GLFWgamepadstate& state)
{
    UNREFERENCED_PARAMETER(state);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void engine::Scene::framebuffer_size_callback(int newWidth, int newHeight)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, newWidth, newHeight);
}

void engine::Scene::refreshFullscreen()
{
    // reinit framebuffers because width and height changed
    m_renderer->initColorFramebuffer(app->width, app->height);
}



void engine::Scene::glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    std::exit(EXIT_FAILURE);
}

// Function to count vertices and polygons
void engine::Scene::beginQuery()
{
    glGenQueries(1, &query);
    glBeginQuery(GL_PRIMITIVES_GENERATED, query);
}

// Function to count vertices and polygons
void engine::Scene::endQuery()
{
    glEndQuery(GL_PRIMITIVES_GENERATED);

    glGetQueryObjectiv(query, GL_QUERY_RESULT, &polycount);

    glDeleteQueries(1, &query);
}

void engine::Scene::countMeshes(std::shared_ptr<Entity>& entity)
{
    if (entity)
    {
        for (auto& child : entity->children)
        {
            meshcount += child->model->numberOfMeshes;
            countMeshes(child);
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
    // render Cube
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

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}