#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene8 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    engine::Text ourText{};

    

public:
    MyScene8(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::BlinnPhong,
                .shadowIntensity = 5.0f
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void init() override
    {
        // cameras
        auto trsCamera1 = engine::Transform{ { 0.0f, 0.0f, 3.0f } };
        auto camera1 = std::make_shared<engine::FlyCamera>(false);
        camera1->zoom = 25.0f;
        camera1->movementSpeed = 10.0f;
        auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(EntityCamera1);






        // light
        auto trsLight1 = engine::Transform{ {0.0f, 1.5f, 0.0f} };
        auto light1 = std::make_shared<engine::PointLight>(0);
        light1->intensity = 1.0f;
        light1->ambientColor = engine::Color(0.5f);

        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);


        auto zzz = engine::ColorManager::hexToNormalizedRGB("#FFF1AD");
        auto zzz2 = engine::Color(0.5f);
        auto zzz3 = engine::Color(0.5f);

        //auto zzz = engine::Color(1.0f, 0.5f, 0.31f, 1.0f); //engine::ColorManager::hexToNormalizedRGB("#FFF1AD");
        //auto zzz2 = engine::Color(1.0f, 0.0f, 0.0f, 1.0f); // engine::ColorManager::hexToNormalizedRGB("#FFFF99");
        //auto zzz3 = engine::Color(0.0f, 1.0f, 0.0f, 1.0f); // engine::ColorManager::hexToNormalizedRGB("#00FFFF");


        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f), engine::UvMapping(6.0f)); //

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(2.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);


        // cube 1
        auto myCube1 = std::make_shared<engine::Cube>();
        myCube1->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

        auto trsCube1 = engine::Transform(glm::vec3(-1.0f, -0.35f, -1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entityCube1 = std::make_shared<engine::Entity>("MyCube1", myCube1, trsCube1);
        getEntityManager().addChild(entityCube1);


        // cube 2
        auto myCube2 = std::make_shared<engine::Cube>();
        myCube2->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

        auto trsCube2 = engine::Transform(glm::vec3(1.0f, -0.35f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entityCube2 = std::make_shared<engine::Entity>("MyCube2", myCube2, trsCube2);
        getEntityManager().addChild(entityCube2);


        // cube 3
        auto myCube3 = std::make_shared<engine::Cube>();
        myCube3->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

        auto trsCube3 = engine::Transform(glm::vec3(1.0f, -0.35f, -1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entityCube3 = std::make_shared<engine::Entity>("MyCube3", myCube3, trsCube3);
        getEntityManager().addChild(entityCube3);


        // cube 4
        auto myCube4 = std::make_shared<engine::Cube>();
        myCube4->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

        auto trsCube4 = engine::Transform(glm::vec3(-1.0f, -0.35f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entityCube4 = std::make_shared<engine::Entity>("MyCube3", myCube4, trsCube4);
        getEntityManager().addChild(entityCube4);


        // sphere
        auto mySphere = std::make_shared<engine::Sphere>();
        mySphere->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

        auto trsSphere = engine::Transform(glm::vec3(0.0f, -0.35f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
        auto entitySphere = std::make_shared<engine::Entity>("MySphere", mySphere, trsSphere);
        getEntityManager().addChild(entitySphere);



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

        getActiveCamera()->processMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

        getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
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
    }

private:
    void drawScene(engine::Shader& shader)
    {
        UNREFERENCED_PARAMETER(shader);
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
