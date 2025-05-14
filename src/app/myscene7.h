#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene7 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";

    std::shared_ptr<engine::PointLight> myPointLight1;
    std::shared_ptr<engine::PointLight> myPointLight2;
    std::shared_ptr<engine::PointLight> myPointLight3;
    std::shared_ptr<engine::PointLight> myPointLight4;


    
    


    engine::Text ourText{};
    engine::Text ourText2{};
    engine::Text textMeshCount{};
    engine::Sprite ourSprite{};


    float rotation{};

    


public:
    MyScene7(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
                .HDRSkyboxBlurStrength = 0.0f,
                .shadowIntensity = 1.0f,
                .iblDiffuseIntensity = 1.0f,
                .iblSpecularIntensity = 1.0f
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void before_init_hook() override
    {
        // do something here if needed
    }

    void init() override
    {
        myPointLight1 = std::make_shared<engine::PointLight>(0);
        myPointLight1->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(-10.0f, 10.0f, 10.0f));

        myPointLight2 = std::make_shared<engine::PointLight>(1);
        myPointLight2->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, 10.0f, 10.0f));

        myPointLight3 = std::make_shared<engine::PointLight>(2);
        myPointLight3->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(-10.0f, -10.0f, 10.0f));

        myPointLight4 = std::make_shared<engine::PointLight>(3);
        myPointLight4->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, -10.0f, 10.0f));

        lights.emplace_back(myPointLight1);
        lights.emplace_back(myPointLight2);
        lights.emplace_back(myPointLight3);
        lights.emplace_back(myPointLight4);
        

        // override default camera properties
        camera.Position = glm::vec3(0.0f, -8.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 100.0f;
        camera.MovementSpeed = 10.0f;


        

        std::shared_ptr<engine::Model> model = std::make_shared<engine::Model>(engine::Model("models/helmet/DamagedHelmet.glTF"));

        {
            std::shared_ptr<engine::Entity> lastEntity = rootEntity;

            float offset = -15.0f;
            for (unsigned int i = 0; i < 8; ++i)
            {
                lastEntity->addChild(std::format("Child{}", i), model);
                lastEntity = lastEntity->children.back();

                //Set transform values
                lastEntity->transform.setLocalPosition({ offset, -10.0f, -10.0f });
                lastEntity->transform.setLocalScale(glm::vec3(2.0f));
                lastEntity->transform.setLocalRotation({ 0.0f, 180.0f, 0.0f });

                offset += 5.0f;
            }
        }
        rootEntity->updateSelfAndChild();



        //float offset = -15.0f;
        //for (unsigned int i = 1; i <= 10; ++i)
        //{
        //    auto trs = engine::Transform{};
        //    trs.setLocalPosition({ offset, -10.0f, -10.0f });
        //    trs.setLocalScale(glm::vec3(2.0f));
        //    trs.setLocalRotation({ 0.0f, 0.0f, 0.0f });


        //    std::shared_ptr<engine::Entity> entity = std::make_shared<engine::Entity>(std::format("Child{}", i), model, trs);

        //    rootEntity->addChild(entity);

        //    offset += 5.0f;
        //}
        //
        //rootEntity->updateSelfAndChild();






        ourText.setup(app->window, FONT_PATH, 28);
        ourText2.setup(app->window, FONT_PATH, 28);
        textMeshCount.setup(app->window, FONT_PATH, 28);
        ourSprite.setup(app->window, "textures/awesomeface.png");
    }


    void after_init_hook() override
    {
        // do something here if needed
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
            camera.ProcessKeyboard(engine::LEFT, deltaTime);
            camera.ProcessKeyboard(engine::YAW_DOWN, deltaTime);
        }

        if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            camera.ProcessKeyboard(engine::RIGHT, deltaTime);
            camera.ProcessKeyboard(engine::YAW_UP, deltaTime);
        }

        if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            camera.ProcessKeyboard(engine::FORWARD, deltaTime);
        }

        if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            camera.ProcessKeyboard(engine::BACKWARD, deltaTime);
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

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

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
        //std::cout <<
        // "Right Trigger/R2: " << state.axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable

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

        //ourText.setup(FONT_PATH, 28, newWidth, newHeight);
        //ourText2.setup(FONT_PATH, 28, newWidth, newHeight);
        //textMeshCount.setup(FONT_PATH, 28, newWidth, newHeight);
        //ourSprite.setup("textures/awesomeface.png", newWidth, newHeight);
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
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };

        // setup lights
        myPointLight1->draw(shader, projection, view, 120.0f);
        myPointLight2->draw(shader, projection, view, 20.0f);
        myPointLight3->draw(shader, projection, view, 20.0f);
        myPointLight4->draw(shader, projection, view, 20.0f);

        rotation += deltaTime * 10.0f;
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        ourText2.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textMeshCount.draw(std::format("{} meshes", (int)meshcount), app->width - 450.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        ourSprite.draw(glm::vec2(50, app->height - 50), glm::vec2(50.0f, -50.0f), 0.0f, glm::vec3(1.0f));
    }
};
