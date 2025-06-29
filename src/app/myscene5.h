#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene5 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    //std::shared_ptr<engine::SpotLight> mySpotLight;


    //engine::Model buddhaModel{};

    //engine::Plane ourPlane{};

    engine::Text ourText{};

    float rotation{};

    


public:
    MyScene5(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
                .shadowIntensity = 0.9f,
                .iblDiffuseIntensity = 1.0f,
                .iblSpecularIntensity = 1.0f
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void init() override
    {
        // cameras
        auto trsCamera1 = engine::Transform{ { 0.0f, 0.0f, 0.0f } };
        auto camera1 = std::make_shared<engine::FlyCamera>(glm::vec3(0.0f, -8.0f, 2.0f), false);
        camera1->Zoom = 25.0f;
        camera1->MovementSpeed = 10.0f;
        auto entityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(entityCamera1);





        // lights
        auto trsLight1 = engine::Transform{ { 0.0f, 0.0f, 0.0f } };
        auto light1 = std::make_shared<engine::SpotLight>(0);
        light1->setIntensity(50.0f);
        light1->cutoff = 12.5f;
        light1->outerCutoff = 17.5f;
        light1->setPosition(glm::vec3(0.0f, 6.0f, 0.0f));
        light1->setTarget(glm::vec3(0.0f, 0.0f, -5.0f));
        light1->setAmbientColor(engine::Color(0.1f, 0.1f, 0.1f, 1.0f));

        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);



        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::Material>(engine::Color(0.2f),
            "textures/pbr/planks/albedo.jpg",
            "",
            "textures/pbr/planks/normal.jpg",
            "textures/pbr/planks/metallic.jpg",
            "textures/pbr/planks/roughness.jpg",
            "textures/pbr/planks/ao.jpg",
            ""), engine::UvMapping(1.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -11.0f, -10.0f), glm::vec3(8.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);




        // buddha model
        std::shared_ptr<engine::Model> cushionModel = std::make_shared<engine::Model>(engine::Model("models/buddha/buddha1.obj"));
        auto trsCushion = engine::Transform(glm::vec3(0.0f, -11.0f + 1.0f, -10.0f), glm::vec3(0.5f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entityCushion = std::make_shared<engine::Entity>("MyCushion", cushionModel, trsCushion);
        getEntityManager().addChild(entityCushion);



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
            getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
        else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
        else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);



        if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::PITCH_UP, deltaTime);
        else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);

        if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::PITCH_DOWN, deltaTime);
        else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
    }


    void mouse_callback(double xposIn, double yposIn)
    {
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

        getActiveCamera()->processMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

        getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
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
        //ourPlane.clean();
        //buddhaModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        auto myCushion = getEntityManager().findEntityByName("MyCushion");
        if (myCushion)
        {
            myCushion->transform.setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
        }

        rotation += deltaTime * 10.0f;


        std::stringstream ss;
        ss << title << " - " << (int)framerate << " FPS";
        app->setWindowTitle(ss.str());
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
