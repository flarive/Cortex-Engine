#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

class MyScene2 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    engine::Text textFPSCount{};
    engine::Text textPolyCount{};
    engine::Text textMeshCount{};
    engine::Text textPrimitiveCount{};

    float rotation{};




public:
    MyScene2(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::BlinnPhong,
                .shadowIntensity = 1.0f
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
        trsCamera1.setLocalPosition({ 0.0f, -16.0f, 2.0f });

        auto camera1 = std::make_shared<engine::FlyCamera>(glm::vec3(0.0f, 0.0f, 3.0f), true);
        camera1->Zoom = 25.0f;
        camera1->MovementSpeed = 10.0f;

        auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(EntityCamera1);


        // lights
        auto trsLight1 = engine::Transform{};
        auto light1 = std::make_shared<engine::SpotLight>(0);
        light1->intensity = 2.0f;
        light1->cutoff = 12.0f;
        light1->outerCutoff = 18.0f;
        light1->position = glm::vec3(0.0f, 1.0f, 3.0f);
        light1->target = glm::vec3(0.0f, 0.0f, 0.0f);
        light1->ambientColor = engine::Color(0.8f, 0.8f, 0.8f, 0.0f);
        
        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);



        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::Material>(engine::Color(0.2f),
            "textures/wood_diffuse.png",
            "textures/wood_specular.png"), engine::UvMapping(2.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, -6.0f), glm::vec3(10.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);




        // cushion model
        auto cushionModel = std::make_shared<engine::Model>(engine::Model("models/cushion/cushion.obj"));
        auto trsCushion = engine::Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f), glm::vec3(0.0f, 45.0f, 0.0f));
        auto entityCushion = std::make_shared<engine::Entity>("MyCushion", cushionModel, trsCushion);
        getEntityManager().addChild(entityCushion);




        textFPSCount.setup(app->window, FONT_PATH, 28);
        textPolyCount.setup(app->window, FONT_PATH, 28);
        textMeshCount.setup(app->window, FONT_PATH, 28);
        textPrimitiveCount.setup(app->window, FONT_PATH, 28);
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
        UNREFERENCED_PARAMETER(xposIn);
        UNREFERENCED_PARAMETER(yposIn);

        //engine::Scene::mouse_callback(xposIn, yposIn);

        //if (show_window)
        //    return;

        //float xpos{ static_cast<float>(xposIn) };
        //float ypos{ static_cast<float>(yposIn) };

        //if (firstMouse)
        //{
        //    lastX = xpos;
        //    lastY = ypos;
        //    firstMouse = false;
        //}

        //float xoffset{ xpos - lastX };
        //float yoffset{ lastY - ypos }; // reversed since y-coordinates go from bottom to top

        //lastX = xpos;
        //lastY = ypos;

        //getActiveCamera()->processMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        UNREFERENCED_PARAMETER(xoffset);
        UNREFERENCED_PARAMETER(yoffset);


        //engine::Scene::scroll_callback(xoffset, yoffset);

        //getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
    }

    void gamepad_callback(const GLFWgamepadstate& state)
    {
        UNREFERENCED_PARAMETER(state);
    }

    void framebuffer_size_callback(int newWidth, int newHeight)
    {
        engine::Scene::framebuffer_size_callback(newWidth, newHeight);

        //ourText.setup(app->window, FONT_PATH, 28);
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
        //ourCube1.clean();
        ////ourSphere1.clean();
        //ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        UNREFERENCED_PARAMETER(shader);

        auto myCushion = getEntityManager().findEntityByName("MyCushion");
        if (myCushion)
        {
            myCushion->transform.setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
        }

        rotation += deltaTime * 10.0f;
    }

    void drawUI()
    {
        // render HUD / UI
        textFPSCount.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textPolyCount.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textMeshCount.draw(std::format("{} meshes", (int)meshcount), app->width - 450.0f, 25.0f, 1.0f, glm::vec3(1.0f));
        textPrimitiveCount.draw(std::format("{} primitives", (int)primitivecount), app->width - 650.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    }
};
