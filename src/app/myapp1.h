#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"


class MyApp1 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    // camera
    engine::Camera cam{ glm::vec3(0.0f, 0.0f, 3.0f), true };

    engine::Model backpackModel{};
    engine::Model cushionModel{};


    engine::PointLight myPointLight{ 0 };
    engine::DirectionalLight myDirectionalLight1{ 0 };
    engine::DirectionalLight myDirectionalLight2{ 1 };
    engine::SpotLight mySpotLight{ 0 };


    engine::Cube ourCube{};
    engine::Plane ourPlane{};
    engine::Billboard ourBillboard{};

    engine::Text ourText{};

    engine::Skybox ourSkybox{};


public:
    MyApp1(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false, engine::RenderMethod _method = engine::RenderMethod::BlinnPhong)
        : engine::App(_title, _width, _height, _fullscreen, _method)
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        // load models
        backpackModel = engine::Model("models/backpack/backpack.obj");
        cushionModel = engine::Model("models/cushion/cushion.obj");

        myPointLight.setup();
        myDirectionalLight1.setup();
        myDirectionalLight2.setup();
        mySpotLight.setup();

        std::vector<std::string> faces
        {
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
        };

        auto zzz{ engine::Material(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png") };
        zzz.setCubeMapTexs(faces);

        ourCube.setup(zzz);
        ourPlane.setup(engine::Material(engine::Color(0.1f), "textures/rusted_metal_diffuse.jpg", "textures/rusted_metal_specular.jpg"), engine::UvMapping(2.0f));
        ourBillboard.setup(engine::Material(engine::Color(0.1f), "textures/grass.png"));

        ourText.setup(width, height);

        ourSkybox.setup(faces);
    }


    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::App::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed =  (mods & GLFW_MOD_SHIFT);

        if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::YAW_DOWN, deltaTime);
        else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::LEFT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::YAW_UP, deltaTime);
        else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::RIGHT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::PITCH_UP, deltaTime);
        else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::FORWARD, deltaTime);

        if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::PITCH_DOWN, deltaTime);
        else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::BACKWARD, deltaTime);
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

        cam.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::App::scroll_callback(xoffset, yoffset);

        cam.ProcessMouseScroll(static_cast<float>(yoffset));
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
        //shader.use();
    
    
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ cam.GetViewMatrix() };
    
    
    
    
        // setup lights
        //ourLights.Draw(lightingShader, projection, view);
        myPointLight.draw(shader, projection, view, 1.0f, glm::vec3(0.0f, 0.3f, 2.0f));
        myDirectionalLight1.draw(shader, projection, view, 1.0f, glm::vec3(2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        myDirectionalLight2.draw(shader, projection, view, 0.2f, glm::vec3(-2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        //mySpotLight.draw(lightingShader, projection, view, 1.0f, cam.Position, cam.Front);
        //mySpotLight.draw(lightingShader, projection, view, 1.0f, glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    
        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", cam.Position);
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
        skyboxReflectShader.setVec3("cameraPos", cam.Position);
    
        // render the loaded model
        backpackModel.draw(skyboxReflectShader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f));
    
    
    
    
    
        // render test cube
        ourCube.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    
        ourBillboard.draw(shader, glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
    
        ourSkybox.draw(projection, view);
    }
    
    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
