#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene4 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    std::shared_ptr<engine::SpotLight> mySpotLight;


    engine::Model cushionModel{};

    engine::Plane ourPlane{};

    engine::Text ourText{};

    float rotation{};

    


public:
    MyScene4(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
                .shadowIntensity = 1.5f,
                .iblDiffuseIntensity = 5.0f,
                .iblSpecularIntensity = 1.0f
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void init() override
    {
        mySpotLight = std::make_shared<engine::SpotLight>(0);
        mySpotLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        mySpotLight->setCutOff(12.5f);
        mySpotLight->setOuterCutOff(17.5f);

        lights.emplace_back(mySpotLight);
        

        // override default camera properties
        camera.Position = glm::vec3(0.0f, -8.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 25.0f;
        camera.MovementSpeed = 10.0f;


        cushionModel = engine::Model("models/cushion/cushion.obj");


        ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/planks/albedo.jpg",
            "",
            "textures/pbr/planks/normal.jpg",
            "textures/pbr/planks/metallic.jpg",
            "textures/pbr/planks/roughness.jpg",
            "textures/pbr/planks/ao.jpg",
            ""), engine::UvMapping(1.0f));

        ourText.setup(app->window, FONT_PATH, 28);
    }



    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::Scene::key_callback(key, scancode, action, mods);

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

        engine::Scene::mouse_callback(xposIn, yposIn);

        if (show_window)
            return;

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
        engine::Scene::scroll_callback(xoffset, yoffset);

        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        UNREFERENCED_PARAMETER(state);
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
        ourPlane.clean();
        cushionModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };


        // setup lights
        mySpotLight->draw(shader, projection, view, 20.0f);


        // render the loaded model
        cushionModel.draw(shader, glm::vec3(0.0f, -9.85f + 2.0f, -10.0f), glm::vec3(1.0f), glm::vec3(0.0f, rotation, 0.0f));


        rotation += deltaTime * 10.0f;

        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -11.00f, -10.0f), glm::vec3(8.0f, 8.0f, 8.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }


};
