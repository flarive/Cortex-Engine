#pragma once

#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/engine.h"

#include "core/include/renderers/blinnphong_renderer.h"


class MyScene1 : public engine::Scene
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    const std::string FONT_PATH = "fonts/Antonio-Regular.ttf";


    engine::Text ourText{};

    engine::Skybox ourSkybox{};

public:
    MyScene1(std::string _title, engine::App* _app)
        : engine::Scene(_title, _app, engine::SceneSettings
            {
                .method = engine::RenderMethod::BlinnPhong
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
        auto trsLight1 = engine::Transform{ { 0.0f, 0.3f, 2.0f } };
        auto light1 = std::make_shared<engine::PointLight>(0);
        light1->setIntensity(5.0f);
        light1->setup();

        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);



        auto trsLight2 = engine::Transform{ { 2.0f, 0.3f, 2.0f } };
        auto light2 = std::make_shared<engine::DirectionalLight>(0);
        light2->setIntensity(1.0f);
        light2->setup();

        auto entityLight2 = std::make_shared<engine::Entity>("Light2", light2, trsLight2);
        getEntityManager().addChild(entityLight2);




        auto trsLight3 = engine::Transform{ { -2.0f, 0.3f, 2.0f } };
        auto light3 = std::make_shared<engine::DirectionalLight>(1);
        light3->setIntensity(1.0f);
        light3->setup();

        auto entityLight3 = std::make_shared<engine::Entity>("Light3", light3, trsLight3);
        getEntityManager().addChild(entityLight3);



        std::vector<std::string> faces
        {
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
        };

        auto zzz{ engine::Material(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png") };
        zzz.setCubeMapTexs(faces);



        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/rusted_metal_diffuse.jpg",
            "textures/rusted_metal_specular.jpg"), engine::UvMapping(1.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);



        // billboard
        auto myBillboard = std::make_shared<engine::Billboard>();
        myBillboard->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/grass.png"), engine::UvMapping(1.0f));

        auto trsBillboard = engine::Transform(glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityBillboard = std::make_shared<engine::Entity>("MyBillboard", myBillboard, trsBillboard);
        getEntityManager().addChild(entityBillboard);




        // cube
        auto myCube = std::make_shared<engine::Cube>();
        myCube->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/container2_diffuse.png",
            "textures/container2_specular.png"));

        auto trsCube = engine::Transform(glm::vec3(0.5f, -0.15f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f, 45.0f, 0.0f));
        auto entityCube = std::make_shared<engine::Entity>("MyCube", myCube, trsCube);
        getEntityManager().addChild(entityCube);






        // cushion model
        std::shared_ptr<engine::Model> cushionModel = std::make_shared<engine::Model>(engine::Model("models/cushion/cushion.obj"));
        auto trsCushion = engine::Transform(glm::vec3(0.0f, -0.20f, 0.0f), glm::vec3(0.10f), glm::vec3(0.0f, 45.0f, 0.0f));
        auto entityCushion = std::make_shared<engine::Entity>("MyCushion", cushionModel, trsCushion);
        getEntityManager().addChild(entityCushion);


        // backpack model
        std::shared_ptr<engine::Model> backpackModel = std::make_shared<engine::Model>(engine::Model("models/backpack/backpack.obj"));
        auto trsBackpack = engine::Transform(glm::vec3(-0.5f, -0.10f, 0.0f), glm::vec3(0.12f), glm::vec3(0.0f, 45.0f, 0.0f));
        auto entityBackpack = std::make_shared<engine::Entity>("MyBackpack", backpackModel, trsBackpack);
        getEntityManager().addChild(entityBackpack);





        ourText.setup(app->window, FONT_PATH, 28);

        ourSkybox.setup(faces);
    }


    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::Scene::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed =  (mods & GLFW_MOD_SHIFT);

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
        ourSkybox.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(getActiveCamera()->Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f)};
        glm::mat4 view{ getActiveCamera()->GetViewMatrix() };
    

    
        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", getActiveCamera()->Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
    
  
        // activate skybox reflection shader
        auto zzz = dynamic_cast<engine::BlinnPhongRenderer*>(getRenderer());
        if (zzz)
        {
            zzz->skyboxShader.use();
            zzz->skyboxShader.setMat4("view", view);
            zzz->skyboxShader.setMat4("projection", projection);
            zzz->skyboxShader.setVec3("cameraPos", getActiveCamera()->Position);
        }

        ourSkybox.draw(projection, view);
    }
    
    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
