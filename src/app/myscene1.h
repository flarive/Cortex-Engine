#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

#include "core/include/renderers/blinnphong_renderer.h"


class MyScene1 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    //engine::Model backpackModel{};
    engine::Model cushionModel{};




    std::shared_ptr<engine::PointLight> myPointLight;
    std::shared_ptr<engine::DirectionalLight> myDirectionalLight1;
    std::shared_ptr<engine::DirectionalLight> myDirectionalLight2;


    engine::Cube ourCube{};
    engine::Plane ourPlane{};
    engine::Billboard ourBillboard{};

    engine::Text ourText{};

    engine::Skybox ourSkybox{};

public:
    MyScene1(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::BlinnPhong
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void init() override
    {
        myPointLight = std::make_shared<engine::PointLight>(0);
        myPointLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.0f, 0.3f, 2.0f));

        myDirectionalLight1 = std::make_shared<engine::DirectionalLight>(0);
        myDirectionalLight1->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));

        myDirectionalLight2 = std::make_shared<engine::DirectionalLight>(1);
        myDirectionalLight2->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(-2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));



        lights.emplace_back(myPointLight);
        lights.emplace_back(myDirectionalLight1);
        lights.emplace_back(myDirectionalLight2);


        // override default camera properties
        camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
        camera.Fps = true;
        camera.Zoom = 25.0f;
        camera.MovementSpeed = 10.0f;

        std::vector<std::string> faces
        {
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
        };

        //auto zzz{ engine::Material(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png") };
        //zzz.setCubeMapTexs(faces);



        ourCube.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/container2_diffuse.png",
            "textures/container2_specular.png"));


        ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/rusted_metal_diffuse.jpg",
            "textures/rusted_metal_specular.jpg"), engine::UvMapping(1.0f));


        // load models
        //backpackModel = engine::Model("models/backpack/backpack.obj");
        cushionModel = engine::Model("models/cushion/cushion.obj");


        

        ourBillboard.setup(std::make_shared<engine::Material>(engine::Color(0.1f), "textures/grass.png"));

        ourText.setup(app->window, FONT_PATH, 28);

        ourSkybox.setup(faces);
    }


    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::Scene::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed =  (mods & GLFW_MOD_SHIFT);

        if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::YAW_DOWN, deltaTime);
        else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::LEFT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::YAW_UP, deltaTime);
        else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::RIGHT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::PITCH_UP, deltaTime);
        else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::FORWARD, deltaTime);

        if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::PITCH_DOWN, deltaTime);
        else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            camera.ProcessKeyboard(engine::BACKWARD, deltaTime);
    }


    void mouse_callback(double xposIn, double yposIn)
    {
        engine::Scene::mouse_callback(xposIn, yposIn);

        if (show_window)
            return;

        float xpos{ static_cast<float>(xposIn) };
        float ypos{ static_cast<float>(yposIn) };

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset{ xpos - lastX };
        float yoffset{ lastY - ypos }; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        engine::Scene::gamepad_callback(state);
    }

    void framebuffer_size_callback(int newWidth, int newHeight)
    {
        engine::Scene::framebuffer_size_callback(newWidth, newHeight);

        ourText.setup(app->window, FONT_PATH, 28);
    }

    void update(engine::Shader& shader) override
    {
        // draw scene and UI in framebuffer
        drawScene(shader);
    }

    void updateUI() override
    {
        drawUI();
    }

    void clean() override
    {
        // clean up any resources
        ourSkybox.clean();
        ourCube.clean();
        ourPlane.clean();
        ourBillboard.clean();
        cushionModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };
    
    
    
    
        // setup lights
        myPointLight->draw(shader, projection, view, 5.0f);
        myDirectionalLight1->draw(shader, projection, view, 1.0f);
        myDirectionalLight2->draw(shader, projection, view, 1.0f);

    
    
    
        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
    
    
        // render primitives
        ourCube.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), glm::vec3(0.0f, 45.0f, 0.0f));
        ourBillboard.draw(shader, glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), glm::vec3(90.0f, 0.0f, 0.0f));
        ourPlane.draw(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

        // render models
        cushionModel.draw(shader, glm::vec3(-1.0f, -0.15f, 0.0f), glm::vec3(0.2f));

        // activate skybox reflection shader
        auto zzz = dynamic_cast<engine::BlinnPhongRenderer*>(getRenderer());
        if (zzz)
        {
            zzz->skyboxShader.use();
            zzz->skyboxShader.setMat4("view", view);
            zzz->skyboxShader.setMat4("projection", projection);
            zzz->skyboxShader.setVec3("cameraPos", camera.Position);
        }

        ourSkybox.draw(projection, view);
    }
    
    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
