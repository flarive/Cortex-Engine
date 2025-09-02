#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene6 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";



    engine::Text ourText{};
    engine::Text ourText2{};
    engine::Sprite ourSprite{};

    

    float rotation{};

    


public:
    MyScene6(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
                .HDRSkyboxBlurStrength = 5.0f,
                .shadowIntensity = 1.0f,
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
        auto trsCamera1 = engine::Transform{ { 0.0f, -16.0f, 8.0f } };
        auto camera1 = std::make_shared<engine::LegacyCamera>(false);
        camera1->zoom = 100.0f;
        camera1->movementSpeed = 10.0f;
        auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(EntityCamera1);






        // lights
        auto trsLight1 = engine::Transform{};
        trsLight1.setLocalPosition({ -10.0f, 10.0f, 10.0f });
        auto light1 = std::make_shared<engine::PointLight>(0);
        light1->intensity = 50.0f;
        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);


        auto trsLight2 = engine::Transform{};
        trsLight2.setLocalPosition({ 10.0f, 10.0f, 10.0f });
        auto light2 = std::make_shared<engine::PointLight>(1);
        light2->intensity = 50.0f;
        auto entityLight2 = std::make_shared<engine::Entity>("Light2", light2, trsLight2);
        getEntityManager().addChild(entityLight2);



        auto trsLight3 = engine::Transform{};
        trsLight3.setLocalPosition({ -10.0f, -10.0f, 10.0f });
        auto light3 = std::make_shared<engine::PointLight>(2);
        light3->intensity = 50.0f;
        auto entityLight3 = std::make_shared<engine::Entity>("Light3", light3, trsLight3);
        getEntityManager().addChild(entityLight3);



        auto trsLight4 = engine::Transform{};
        trsLight4.setLocalPosition({ 10.0f, -10.0f, 10.0f });
        auto light4 = std::make_shared<engine::PointLight>(3);
        light4->intensity = 50.0f;
        auto entityLight4 = std::make_shared<engine::Entity>("Light4", light4, trsLight4);
        getEntityManager().addChild(entityLight4);






        // helmet model
        std::shared_ptr<engine::Model> helmetModel = std::make_shared<engine::Model>("models/helmet/DamagedHelmet.glTF", false, true);
        auto trsHelmet = engine::Transform(glm::vec3(0.0f, -15.0f, -10.0f), glm::vec3(4.0f), glm::vec3(0.0f, 180.0f, 0.0f));
        auto entityHelmet = std::make_shared<engine::Entity>("MyHelmet", helmetModel, trsHelmet);
        getEntityManager().addChild(entityHelmet);

        auto helmetMat = helmetModel->meshes[0].getMaterial();
        if (helmetMat)
        {
            helmetMat->setAmbientIntensity(5.0f);
            helmetMat->setEmissiveIntensity(5.0f);
        }


        ourText.setup(app->window, FONT_PATH, 28);
        ourText2.setup(app->window, FONT_PATH, 28);
        ourSprite.setup(app->window, "textures/awesomeface.png");
    }



    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::Scene::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);
            getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
        }



        if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);
            getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
        }


        if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);
        }

        if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
        }
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
        //helmetModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        UNREFERENCED_PARAMETER(shader);

        auto MyHelmet = getEntityManager().findEntityByName("MyHelmet");
        if (MyHelmet)
        {
            glm::vec3 zzz = MyHelmet->transform.getLocalRotation();
            MyHelmet->transform.setLocalRotation(glm::vec3(zzz.x, zzz.y + rotation, zzz.z));
        }

        rotation += deltaTime * 0.002f;
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        ourText2.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        ourSprite.draw(glm::vec2(50, app->height - 100), glm::vec2(50.0f, 50.0f), 0.0f, glm::vec3(1.0f));
    }
};
