#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp4 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };



    engine::PointLight myPointLight{ 0 };
    engine::DirectionalLight myDirectionalLight{ 0 };
    engine::SpotLight mySpotLight{ 0 };

    engine::Model cushionModel{};

    engine::Cube ourCube1{};

    engine::Plane ourPlane{};

    engine::Text ourText{};

    float rotation{};


public:
    MyApp4(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen, engine::RenderMethod::PBR)
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        setLightPosition(glm::vec3(0.0f, 5.0f, -2.0f));
        setLightTarget(glm::vec3(0.0f, 0.0f, -5.0f));

        myPointLight.setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f });
        myDirectionalLight.setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f });
        mySpotLight.setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f });
        mySpotLight.setCutOff(8.0f);
        mySpotLight.setOuterCutOff(24.f);
        camera.MovementSpeed = 10.0f;

        // override default camera properties
        camera.Position = glm::vec3(0.0f, -8.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 25.0f;


        cushionModel = engine::Model("models/cushion/cushion.obj");


        ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/planks/albedo.jpg",
            "",
            "textures/pbr/planks/normal.jpg",
            "textures/pbr/planks/metallic.jpg",
            "textures/pbr/planks/roughness.jpg",
            "textures/pbr/planks/ao.jpg",
            "textures/pbr/planks/displace.jpg"), engine::UvMapping(1.0f));

        //ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
        //    "textures/wood_diffuse.png",
        //    "textures/wood_specular.png"), engine::UvMapping(2.0f));

        ourText.setup(width, height);
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::App::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed = (mods & GLFW_MOD_SHIFT);

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
        //UNREFERENCED_PARAMETER(xposIn);
        //UNREFERENCED_PARAMETER(yposIn);

        engine::App::mouse_callback(xposIn, yposIn);

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::App::scroll_callback(xoffset, yoffset);

        camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
        ourCube1.clean();
        //ourSphere1.clean();
        ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };


        // setup lights
        //myPointLight.draw(shader, projection, view, 20.0f, getLightPosition());
        //myDirectionalLight.draw(shader, projection, view, 1.0f, getLightPosition(), getLightTarget());
        mySpotLight.draw(shader, projection, view, 20.0f, getLightPosition(), getLightTarget());



        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);



        // render the loaded model
        cushionModel.draw(shader, glm::vec3(0.0f, -8.75f, -10.0f), glm::vec3(2.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));


        rotation += deltaTime * 10.0f;

        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -11.00f, -10.0f), glm::vec3(12.0f, 12.0f, 12.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
