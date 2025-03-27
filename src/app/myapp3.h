#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp3 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    std::shared_ptr<engine::PointLight> myPointLight1;
    std::shared_ptr<engine::PointLight> myPointLight2;
    std::shared_ptr<engine::PointLight> myPointLight3;
    std::shared_ptr<engine::PointLight> myPointLight4;

    std::shared_ptr<engine::DirectionalLight> myDirectionalLight;


    engine::Sphere redSciFiMetalSphere{};
    engine::Sphere rustedIronSphere{};
    engine::Sphere goldSphere{};
    engine::Sphere grassSphere{};
    engine::Sphere plasticSphere{};
    engine::Sphere wallSphere{};
    engine::Sphere bronzeSphere{};

    engine::Model cushionModel{};
    //engine::Cube ourCube{};

    engine::Plane ourPlane{};


    engine::Text ourText{};


    engine::Shader lightCubeShader{};

    float rotation{};

public:
    MyApp3(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen, engine::AppSettings
            {
                engine::RenderMethod::PBR,
                false,
                "textures/hdr/newport_loft.hdr",
                1.5f,
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
        myPointLight1 = std::make_shared<engine::PointLight>(0);
        myPointLight1->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(0.5f, 1.0f, 0.3f)); //glm::vec3(-10.0f, 10.0f, 10.0f));

        myPointLight2 = std::make_shared<engine::PointLight>(1);
        myPointLight2->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, 10.0f, 10.0f));

        myPointLight3 = std::make_shared<engine::PointLight>(2);
        myPointLight3->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(-10.0f, -10.0f, 10.0f));

        myPointLight4 = std::make_shared<engine::PointLight>(3);
        myPointLight4->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, -10.0f, 10.0f));


        myDirectionalLight = std::make_shared<engine::DirectionalLight>(0);
        myDirectionalLight->setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, glm::vec3(10.0f, -10.0f, 10.0f));


        lights.emplace_back(myPointLight1);
        lights.emplace_back(myPointLight2);
        lights.emplace_back(myPointLight3);
        lights.emplace_back(myPointLight4);
        lights.emplace_back(myDirectionalLight);
        


        // override default camera properties
        camera.Position = glm::vec3(0.0f, -5.0f, 2.0f);
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




        lightCubeShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");

        cushionModel = engine::Model("models/cushion/cushion.obj");

        //ourCube.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
        //    "models/sphere/rounded-metal-cubes/albedo.png",
        //    "",
        //    "models/sphere/rounded-metal-cubes/normal.png",
        //    "models/sphere/rounded-metal-cubes/metallic.png",
        //    "models/sphere/rounded-metal-cubes/roughness.png",
        //    "models/sphere/rounded-metal-cubes/ao.png",
        //    "models/sphere/rounded-metal-cubes/height.png"), engine::UvMapping(2.0f));



        //redSciFiMetalSphere = engine::Model("models/sphere/smooth_sphere_80.obj");

        redSciFiMetalSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "models/sphere/rounded-metal-cubes/albedo.dds",
            "",
            "models/sphere/rounded-metal-cubes/normal.png",
            "models/sphere/rounded-metal-cubes/metallic.png",
            "models/sphere/rounded-metal-cubes/roughness.png",
            "models/sphere/rounded-metal-cubes/ao.png",
            "models/sphere/rounded-metal-cubes/height.png"), engine::UvMapping(3.0f));

        //redSciFiMetalSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
        //    "textures/bricks2.jpg",
        //    "",
        //    "textures/bricks2_normal.jpg",
        //    "",
        //    "",
        //    "",
        //    "textures/bricks2_disp.jpg"), engine::UvMapping(3.0f));

        auto mat = redSciFiMetalSphere.getMaterial();
        if (mat)
        {
            mat->setNormalIntensity(5.0f);
            mat->setHeightIntensity(0.0f);
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

        bronzeSphere.setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/pbr/bronze/albedo.png",
            "",
            "textures/pbr/bronze/normal.png",
            "textures/pbr/bronze/metallic.png",
            "textures/pbr/bronze/roughness.png",
            "textures/pbr/bronze/ao.png", "", 0.5f));


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

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        engine::App::gamepad_callback(state);

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
        rustedIronSphere.clean();
        goldSphere.clean();
        grassSphere.clean();
        plasticSphere.clean();
        wallSphere.clean();
        bronzeSphere.clean();
        ourPlane.clean();
        cushionModel.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // render test sphere
        redSciFiMetalSphere.draw(shader, glm::vec3(-7.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        rustedIronSphere.draw(shader, glm::vec3(-5.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        goldSphere.draw(shader, glm::vec3(-3.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        grassSphere.draw(shader, glm::vec3(-1.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        plasticSphere.draw(shader, glm::vec3(1.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        wallSphere.draw(shader, glm::vec3(3.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        bronzeSphere.draw(shader, glm::vec3(5.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));


        


        ourPlane.draw(shader, glm::vec3(0.0f, -15.0f, -15.0f), glm::vec3(12.0f, 12.0f, 12.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        // render the loaded model
        //cushionModel.draw(shader, glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.5f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        // render test cube
        //ourCube.draw(shader, glm::vec3(0.0f, -10.0f, -10.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));



        rotation += deltaTime * 10.0f;


        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };

        // setup lights
        myPointLight1->draw(shader, projection, view, 20.0f, myPointLight1->getPosition(), myPointLight1->getTarget()); // ????????????
        myPointLight2->draw(shader, projection, view, 20.0f, myPointLight2->getPosition(), myPointLight2->getTarget()); // ????????????
        myPointLight3->draw(shader, projection, view, 20.0f, myPointLight3->getPosition(), myPointLight3->getTarget()); // ????????????
        myPointLight4->draw(shader, projection, view, 20.0f, myPointLight4->getPosition(), myPointLight4->getTarget()); // ????????????
        myDirectionalLight->draw(shader, projection, view, 1.0f, myDirectionalLight->getPosition(), myDirectionalLight->getTarget());
        //mySpotLight.draw(shader, projection, view, 20.0f, getLightPosition(), getLightTarget());
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
