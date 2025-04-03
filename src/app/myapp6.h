#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp6 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };




    std::shared_ptr<engine::SpotLight> mySpotLight;


    engine::Model buddhaModel{};

    engine::Plane ourPlane{};

    engine::Text ourText{};

    float rotation{};

    


public:
    MyApp6(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen, engine::AppSettings
            {
                engine::RenderMethod::PBR,
                true,
                "textures/hdr/blue_photo_studio_2k.hdr",
                0.9f,
                1.0f,
                1.0f
            })
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        mySpotLight = std::make_shared<engine::SpotLight>(0);
        mySpotLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        mySpotLight->setCutOff(12.5f);
        mySpotLight->setOuterCutOff(17.5f);

        lights.emplace_back(mySpotLight);
        

        // override default camera properties
        camera.Position = glm::vec3(0.0f, -8.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 25.0f;
        camera.MovementSpeed = 10.0f;


        buddhaModel = engine::Model("models/helmet/DamagedHelmet.gltf");


        ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/planks/albedo.jpg",
            "",
            "textures/pbr/planks/normal.jpg",
            "textures/pbr/planks/metallic.jpg",
            "textures/pbr/planks/roughness.jpg",
            "textures/pbr/planks/ao.jpg",
            ""), engine::UvMapping(1.0f));

        ourText.setup(width, height);

        after_init();
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

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        camera.ProcessJoystickMovement(state);

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
        ourPlane.clean();
        buddhaModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };


        shader.use();
        shader.setVec3("viewPos", camera.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);



        // render the loaded model
        buddhaModel.draw(shader, glm::vec3(0.0f, -11.0f + 1.0f, -10.0f), glm::vec3(0.5f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -11.00f, -10.0f), glm::vec3(8.0f, 8.0f, 8.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        // setup lights
        mySpotLight->draw(shader, projection, view, 50.0f);

        rotation += deltaTime * 10.0f;
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
