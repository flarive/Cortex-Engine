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

    /*std::shared_ptr<engine::SpotLight> mySpotLight;




    engine::Model cushionModel{};

    engine::Cube ourCube1{};

    engine::Plane ourPlane{};*/

    engine::Text ourText{};

    float rotation{};


public:
    MyScene2(std::string _title, engine::App* _app)
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
        auto trsCamera1 = engine::Transform{ { 0.0f, -16.0f, 2.0f } };
        auto camera1 = std::make_shared<engine::FlyCamera>(glm::vec3(0.0f, 0.0f, 3.0f), true);
        camera1->Zoom = 25.0f;
        camera1->MovementSpeed = 10.0f;
        auto entityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
        getEntityManager().addChild(entityCamera1);


        // lights
        auto trsLight1 = engine::Transform{ { 0.0f, 1.0f, 3.0f} };
        auto light1 = std::make_shared<engine::SpotLight>(0);
        light1->setIntensity(2.0f);
        // position : glm::vec3(0.0f, 1.0f, 3.0f)
        // target : glm::vec3(0.0f, 0.0f, 1.0f)
        light1->setup();
        //mySpotLight->draw(shader, projection, view, engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, 2.0f, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
        getEntityManager().addChild(entityLight1);












        // ground
        auto myPlane = std::make_shared<engine::Plane>();
        myPlane->setup(std::make_shared<engine::Material>(engine::Color(0.1f),
            "textures/rusted_metal_diffuse.jpg",
            "textures/rusted_metal_specular.jpg"), engine::UvMapping(1.0f));

        auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f), glm::vec3(90.0f, 0.0f, 0.0f));
        auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
        getEntityManager().addChild(entityPlane);




        // cushion model
        std::shared_ptr<engine::Model> cushionModel = std::make_shared<engine::Model>(engine::Model("models/cushion/cushion.obj"));
        auto trsCushion = engine::Transform(glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.3f), glm::vec3(0.0f, 45.0f, 0.0f));
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
        UNREFERENCED_PARAMETER(xposIn);
        UNREFERENCED_PARAMETER(yposIn);

        //engine::App::mouse_callback(xposIn, yposIn);

        //if (show_window)
        //    return;

        //float xpos = static_cast<float>(xposIn);
        //float ypos = static_cast<float>(yposIn);

        //if (firstMouse)
        //{
        //    lastX = xpos;
        //    lastY = ypos;
        //    firstMouse = false;
        //}

        //float xoffset = xpos - lastX;
        //float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        //lastX = xpos;
        //lastY = ypos;

        //cam.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        engine::Scene::scroll_callback(xoffset, yoffset);

        getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
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
        //ourCube1.clean();
        ////ourSphere1.clean();
        //ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(getActiveCamera()->Zoom), (float)app->width / (float)app->height, 0.1f, 100.0f) };
        glm::mat4 view{ getActiveCamera()->GetViewMatrix() };


        // draw lights
        //mySpotLight->draw(shader, projection, view, engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f }, 2.0f, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        

        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", getActiveCamera()->Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);


        // render the loaded model
        //cushionModel.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.3f), glm::vec3(0.0f, rotation, 0.0f));



        // render test cube
        //ourCube1.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //ourSphere1.draw(blinnPhongShader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 0.0f, glm::vec3(1.0f, 1.0f, 0.0f));

        rotation += deltaTime * 10.0f;

        // render test plane
        //ourPlane.draw(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
