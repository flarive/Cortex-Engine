#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene3 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";

 

    engine::Text ourText{};


    float rotation{};

public:
    MyScene3(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::PBR,
                .HDRSkyboxHide = false,
                .HDRSkyboxFilePath = "textures/hdr/newport_loft.hdr",
                .shadowIntensity = 1.5f,
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
        auto trsCamera1 = engine::Transform{};
        trsCamera1.setLocalPosition({ 0.0f, -12.0f, 2.0f });

        auto camera1 = std::make_shared<engine::FlyCamera>(glm::vec3(0.0f, -12.0f, 2.0f), false);
        camera1->zoom = 75;
        camera1->movementSpeed = 10.0f;

        auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(EntityCamera1);



        // lights
        auto trsLight5 = engine::Transform{ { 0.0f, 4.0f, -2.0f } };
        auto light5 = std::make_shared<engine::SpotLight>(0);
        light5->intensity = 50.0f;
        light5->cutoff = 12.5f;
        light5->outerCutoff = 27.5f;
        light5->target = glm::vec3(0.0f, 0.0f, -4.0f);
        auto entityLight5 = std::make_shared<engine::Entity>("Light5", light5, trsLight5);
        getEntityManager().addChild(entityLight5);




        auto trsLight1 = engine::Transform{ { -10.0f, 10.0f, 10.0f } };
        auto light1 = std::make_shared<engine::PointLight>(0);
        light1->intensity = 50.0f;
        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);



        auto trsLight2 = engine::Transform{ { 10.0f, 10.0f, 10.0f } };
        auto light2 = std::make_shared<engine::PointLight>(1);
        light2->intensity = 50.0f;
        auto entityLight2 = std::make_shared<engine::Entity>("Light2", light2, trsLight2);
        getEntityManager().addChild(entityLight2);



        auto trsLight3 = engine::Transform{ { -10.0f, -10.0f, 10.0f } };
        auto light3 = std::make_shared<engine::PointLight>(2);
        light3->intensity = 50.0f;
        auto entityLight3 = std::make_shared<engine::Entity>("Light3", light3, trsLight3);
        getEntityManager().addChild(entityLight3);



        auto trsLight4 = engine::Transform{ { 10.0f, -10.0f, 10.0f } };
        auto light4 = std::make_shared<engine::PointLight>(3);
        light4->intensity = 50.0f;
        auto entityLight4 = std::make_shared<engine::Entity>("Light4", light4, trsLight4);
        getEntityManager().addChild(entityLight4);




        


        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        auto matPlane = std::make_shared<engine::Material>(engine::Color(1.0f),
            "models/sphere/cliff/albedo.jpg",
            "",
            "models/sphere/cliff/normal.jpg",
            "models/sphere/cliff/metallic.jpg",
            "models/sphere/cliff/roughness.jpg",
            "models/sphere/cliff/ao.jpg",
            "models/sphere/cliff/height.jpg");
        matPlane->setNormalIntensity(1.0f);

        myPlane->setup(matPlane, engine::UvMapping(2.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -15.0f, -15.0f), glm::vec3(12.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);



        // sphere models
        auto redSciFiMetalSphere = std::make_shared<engine::Sphere>();

        auto matSphere1 = std::make_shared<engine::Material>(engine::Color(0.1f),
            "models/sphere/rounded-metal-cubes/albedo.dds",
            "",
            "models/sphere/rounded-metal-cubes/normal.png",
            "models/sphere/rounded-metal-cubes/metallic.png",
            "models/sphere/rounded-metal-cubes/roughness.png",
            "models/sphere/rounded-metal-cubes/ao.png",
            "models/sphere/rounded-metal-cubes/height.png");
        matSphere1->setNormalIntensity(5.0f);

        redSciFiMetalSphere->setup(matSphere1, engine::UvMapping(2.0f));

        auto trsSphere1 = engine::Transform(glm::vec3(-5.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere1 = std::make_shared<engine::Entity>("MySphere1", redSciFiMetalSphere, trsSphere1);
        getEntityManager().addChild(entitySphere1);




        auto rustedIronSphere = std::make_shared<engine::Sphere>();
        rustedIronSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/rusted_iron/albedo.png",
            "",
            "textures/pbr/rusted_iron/normal.png",
            "textures/pbr/rusted_iron/metallic.png",
            "textures/pbr/rusted_iron/roughness.png",
            "textures/pbr/rusted_iron/ao.png",
            "textures/pbr/rusted_iron/height.png"));

        auto trsSphere2 = engine::Transform(glm::vec3(-3.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere2 = std::make_shared<engine::Entity>("MySphere2", rustedIronSphere, trsSphere2);
        getEntityManager().addChild(entitySphere2);



        auto goldSphere = std::make_shared<engine::Sphere>();
        goldSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/gold/albedo.png",
            "",
            "textures/pbr/gold/normal.png",
            "textures/pbr/gold/metallic.png",
            "textures/pbr/gold/roughness.png",
            "textures/pbr/gold/ao.png"));

        auto trsSphere3 = engine::Transform(glm::vec3(-1.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere3 = std::make_shared<engine::Entity>("MySphere3", goldSphere, trsSphere3);
        getEntityManager().addChild(entitySphere3);



        auto grassSphere = std::make_shared<engine::Sphere>();
        grassSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/grass/albedo.png",
            "",
            "textures/pbr/grass/normal.png",
            "textures/pbr/grass/metallic.png",
            "textures/pbr/grass/roughness.png",
            "textures/pbr/grass/ao.png"));

        auto trsSphere4 = engine::Transform(glm::vec3(1.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere4 = std::make_shared<engine::Entity>("MySphere3", grassSphere, trsSphere4);
        getEntityManager().addChild(entitySphere4);



        auto plasticSphere = std::make_shared<engine::Sphere>();
        plasticSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/plastic/albedo.png",
            "",
            "textures/pbr/plastic/normal.png",
            "textures/pbr/plastic/metallic.png",
            "textures/pbr/plastic/roughness.png",
            "textures/pbr/plastic/ao.png", "", 1.0f));

        auto trsSphere5 = engine::Transform(glm::vec3(3.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere5 = std::make_shared<engine::Entity>("MySphere4", plasticSphere, trsSphere5);
        getEntityManager().addChild(entitySphere5);



        auto wallSphere = std::make_shared<engine::Sphere>();
        wallSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/wall/albedo.png",
            "",
            "textures/pbr/wall/normal.png",
            "textures/pbr/wall/metallic.png",
            "textures/pbr/wall/roughness.png",
            "textures/pbr/wall/ao.png"), engine::UvMapping(1.0f));

        auto trsSphere6 = engine::Transform(glm::vec3(5.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere6 = std::make_shared<engine::Entity>("MySphere5", wallSphere, trsSphere6);
        getEntityManager().addChild(entitySphere6);


        //auto wallPlane = wallSphere.getMaterial();
        //if (wallPlane)
        //{
        //    wallPlane->setNormalIntensity(1.0f);
        //}



        auto bronzeSphere = std::make_shared<engine::Sphere>();
        bronzeSphere->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/bronze/albedo.png",
            "",
            "textures/pbr/bronze/normal.png",
            "textures/pbr/bronze/metallic.png",
            "textures/pbr/bronze/roughness.png",
            "textures/pbr/bronze/ao.png"), engine::UvMapping(1.0f));

        auto trsSphere7 = engine::Transform(glm::vec3(7.0f, -14.0f, -10.0f), glm::vec3(1.0f));
        auto entitySphere7 = std::make_shared<engine::Entity>("MySphere6", bronzeSphere, trsSphere7);
        getEntityManager().addChild(entitySphere7);

        //auto bronzeMat = bronzeSphere.getMaterial();
        //if (bronzeMat)
        //{
        //    bronzeMat->setAmbientIntensity(5.0f);
        //}


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
        //rustedIronSphere.clean();
        //goldSphere.clean();
        //grassSphere.clean();
        //plasticSphere.clean();
        //wallSphere.clean();
        //bronzeSphere.clean();
        //ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        UNREFERENCED_PARAMETER(shader);

        rotation += deltaTime * 10.0f;
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
