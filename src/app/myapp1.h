#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"


class MyApp1 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    engine::Model backpackModel{};
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
    MyApp1(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen, engine::AppSettings
            {
                engine::RenderMethod::BlinnPhong
            })
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        // load models
        //backpackModel = engine::Model("models/backpack/backpack.obj");
        cushionModel = engine::Model("models/cushion/cushion.obj");


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
            "textures/rusted_metal_specular.jpg"), engine::UvMapping(4.0f));

        

        ourBillboard.setup(std::make_shared<engine::Material>(engine::Color(0.1f), "textures/grass.png"));

        ourText.setup(width, height);

        ourSkybox.setup(faces);

        after_init();
    }


    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::App::key_callback(key, scancode, action, mods);

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
        engine::App::mouse_callback(xposIn, yposIn);

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
        engine::App::scroll_callback(xoffset, yoffset);

        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        engine::App::gamepad_callback(state);

        

    }

    void framebuffer_size_callback(int newWidth, int newHeight)
    {
        engine::App::framebuffer_size_callback(newWidth, newHeight);

        ourText.setup(newWidth, newHeight);
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
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };
    
    
    
    
        // setup lights
        myPointLight->draw(shader, projection, view, 1.0f, myPointLight->getPosition());
        myDirectionalLight1->draw(shader, projection, view, 1.0f, myDirectionalLight1->getPosition(), myDirectionalLight1->getTarget());
        myDirectionalLight2->draw(shader, projection, view, 1.0f, myDirectionalLight2->getPosition(), myDirectionalLight2->getTarget());

    
    
    
        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
    
    
    
    
    
        // render the loaded model
        //glm::mat4 model1 { glm::mat4(1.0f) };
        //model1 = glm::translate(model1, glm::vec3(0.0f, -0.2f, 0.0f)); // translate it down so it's at the center of the scene
        //model1 = glm::scale(model1, glm::vec3(0.3f));	// it's a bit too big for our scene, so scale it down
        //model1 = glm::rotate(model1, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //shader.setMat4("model", model1);
        //cushionModel.draw(shader);
    
    
        // activate skybox reflection shader
        skyboxReflectShader.use();
        skyboxReflectShader.setMat4("view", view);
        skyboxReflectShader.setMat4("projection", projection);
        skyboxReflectShader.setVec3("cameraPos", camera.Position);
    
        // render the loaded model
        //backpackModel.draw(skyboxReflectShader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f));
    
    
    
    
    
        // render test cube
        ourCube.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    
        ourBillboard.draw(shader, glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), -90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
    
        ourSkybox.draw(projection, view);
    }
    
    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
