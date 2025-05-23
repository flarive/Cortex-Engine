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

    std::shared_ptr<engine::PointLight> myPointLight1{};
    std::shared_ptr<engine::PointLight> myPointLight2{};
    std::shared_ptr<engine::PointLight> myPointLight3{};
    std::shared_ptr<engine::PointLight> myPointLight4{};

    std::shared_ptr<engine::DirectionalLight> myDirectionalLight{};

    std::shared_ptr<engine::SpotLight> mySpotLight{};



    engine::Sphere redSciFiMetalSphere{};
    engine::Sphere rustedIronSphere{};
    engine::Sphere goldSphere{};
    engine::Sphere grassSphere{};
    engine::Sphere plasticSphere{};
    engine::Sphere wallSphere{};
    engine::Sphere bronzeSphere{};


    engine::Plane ourPlane{};


    engine::Text ourText{};


    engine::Shader lightCubeShader{};

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
                .iblSpecularIntensity = 1.0f,
                .applyGammaCorrection = true
            })
    {
        // my application specific state gets initialized here

        lastX = app->width / 2.0f;
        lastY = app->height / 2.0f;
    }

    void init() override
    {
        myPointLight1 = std::make_shared<engine::PointLight>(0);
        myPointLight1->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.5f, 1.0f, 0.3f)); //glm::vec3(-10.0f, 10.0f, 10.0f));

        myPointLight2 = std::make_shared<engine::PointLight>(1);
        myPointLight2->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, 10.0f, 10.0f));

        myPointLight3 = std::make_shared<engine::PointLight>(2);
        myPointLight3->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(-10.0f, -10.0f, 10.0f));

        myPointLight4 = std::make_shared<engine::PointLight>(3);
        myPointLight4->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, -10.0f, 10.0f));


        //myDirectionalLight = std::make_shared<engine::DirectionalLight>(0);
        //myDirectionalLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, -10.0f, 10.0f));

        mySpotLight = std::make_shared<engine::SpotLight>(0);
        mySpotLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.0f, 4.0f, -2.0f), glm::vec3(0.0f, 0.0f, -4.0f));
        mySpotLight->setCutOff(12.5f);
        mySpotLight->setOuterCutOff(27.5f);


        lights.emplace_back(myPointLight1);
        lights.emplace_back(myPointLight2);
        lights.emplace_back(myPointLight3);
        lights.emplace_back(myPointLight4);
        lights.emplace_back(mySpotLight);





        // override default camera properties
        camera.Position = glm::vec3(0.0f, -12.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 75.0f;
        camera.MovementSpeed = 10.0f;

        ourPlane.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "models/sphere/cliff/albedo.jpg",
            "",
            "models/sphere/cliff/normal.jpg",
            "models/sphere/cliff/metallic.jpg",
            "models/sphere/cliff/roughness.jpg",
            "models/sphere/cliff/ao.jpg",
            "models/sphere/cliff/height.jpg"), engine::UvMapping(2.0f));

        auto matPlane = ourPlane.getMaterial();
        if (matPlane)
        {
            matPlane->setNormalIntensity(1.0f);
            //matPlane->setAmbientIntensity(10.0f);
        }


        lightCubeShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");



        //redSciFiMetalSphere = engine::Model("models/sphere/smooth_sphere_80.obj");

        redSciFiMetalSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "models/sphere/rounded-metal-cubes/albedo.dds",
            "",
            "models/sphere/rounded-metal-cubes/normal.png",
            "models/sphere/rounded-metal-cubes/metallic.png",
            "models/sphere/rounded-metal-cubes/roughness.png",
            "models/sphere/rounded-metal-cubes/ao.png",
            "models/sphere/rounded-metal-cubes/height.png"), engine::UvMapping(3.0f));

        auto mat = redSciFiMetalSphere.getMaterial();
        if (mat)
        {
            mat->setNormalIntensity(5.0f);
        }


        rustedIronSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/rusted_iron/albedo.png",
            "",
            "textures/pbr/rusted_iron/normal.png",
            "textures/pbr/rusted_iron/metallic.png",
            "textures/pbr/rusted_iron/roughness.png",
            "textures/pbr/rusted_iron/ao.png",
            "textures/pbr/rusted_iron/height.png"));

        goldSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/gold/albedo.png",
            "",
            "textures/pbr/gold/normal.png",
            "textures/pbr/gold/metallic.png",
            "textures/pbr/gold/roughness.png",
            "textures/pbr/gold/ao.png"));

        grassSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/grass/albedo.png",
            "",
            "textures/pbr/grass/normal.png",
            "textures/pbr/grass/metallic.png",
            "textures/pbr/grass/roughness.png",
            "textures/pbr/grass/ao.png"));

        plasticSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/plastic/albedo.png",
            "",
            "textures/pbr/plastic/normal.png",
            "textures/pbr/plastic/metallic.png",
            "textures/pbr/plastic/roughness.png",
            "textures/pbr/plastic/ao.png", "", 1.0f));


        wallSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/wall/albedo.png",
            "",
            "textures/pbr/wall/normal.png",
            "textures/pbr/wall/metallic.png",
            "textures/pbr/wall/roughness.png",
            "textures/pbr/wall/ao.png"), engine::UvMapping(1.0f));

        auto wallPlane = wallSphere.getMaterial();
        if (wallPlane)
        {
            wallPlane->setNormalIntensity(1.0f);
        }

        bronzeSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f, 1.0f, 0.1f, 1.0f),
            "textures/pbr/bronze/albedo.png",
            "",
            "textures/pbr/bronze/normal.png",
            "textures/pbr/bronze/metallic.png",
            "textures/pbr/bronze/roughness.png",
            "textures/pbr/bronze/ao.png", "", 0.0f));


        auto bronzeMat = bronzeSphere.getMaterial();
        if (bronzeMat)
        {
            bronzeMat->setAmbientIntensity(5.0f);
        }


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
        engine::Scene::gamepad_callback(state);

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
        rustedIronSphere.clean();
        goldSphere.clean();
        grassSphere.clean();
        plasticSphere.clean();
        wallSphere.clean();
        bronzeSphere.clean();
        ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // render test sphere
        redSciFiMetalSphere.draw(shader, glm::vec3(-7.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        rustedIronSphere.draw(shader, glm::vec3(-5.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        goldSphere.draw(shader, glm::vec3(-3.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        grassSphere.draw(shader, glm::vec3(-1.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        plasticSphere.draw(shader, glm::vec3(1.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        wallSphere.draw(shader, glm::vec3(3.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));
        bronzeSphere.draw(shader, glm::vec3(5.0f, -14.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, rotation, 0.0f));


        


        ourPlane.draw(shader, glm::vec3(0.0f, -15.0f, -15.0f), glm::vec3(12.0f, 12.0f, 12.0f), glm::vec3(90.0f, 0.0f, 0.0f));



        rotation += deltaTime * 10.0f;


        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };




        // setup lights
        myPointLight1->draw(shader, projection, view, 50.0f); // ????????????
        myPointLight2->draw(shader, projection, view, 50.0f); // ????????????
        myPointLight3->draw(shader, projection, view, 50.0f); // ????????????
        myPointLight4->draw(shader, projection, view, 50.0f); // ????????????
        //myDirectionalLight->draw(shader, projection, view, 1.0f, myDirectionalLight->getPosition(), myDirectionalLight->getTarget());
        //mySpotLight.draw(shader, projection, view, 20.0f, getLightPosition(), getLightTarget());

        // setup lights
        mySpotLight->draw(shader, projection, view, 20.0f); // ???????????????
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
