#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp2 : public engine::App
{
public:
    MyApp2(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen)
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        setLightPosition(glm::vec3(0.0f, 1.0f, 3.0f));
        setLightTarget(glm::vec3(0.0f, 0.0f, 1.0f));

        //myPointLight.setup();
        //myDirectionalLight.setup();
        mySpotLight.setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f });
        mySpotLight.setCutOff(8.0f);
        mySpotLight.setOuterCutOff(20.f);

        cushionModel = engine::Model("models/cushion/cushion.obj");

        ourCube1.setup(engine::Material(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png"));// , "textures/container2_normal.png"));
        //ourCube2.setup(engine::Material(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png"));

        ourPlane.setup(engine::Material(engine::Color(0.1f), "textures/wood_diffuse.png", "textures/wood_specular.png"), engine::UvMapping(2.0f));

        ourText.setup(width, height);
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void key_callback(int key, int scancode, int action, int mods)
    {
        engine::App::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::YAW_DOWN, deltaTime);
        else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::LEFT, deltaTime);

        if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::YAW_UP, deltaTime);
        else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::RIGHT, deltaTime);



        if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::PITCH_UP, deltaTime);
        else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::FORWARD, deltaTime);

        if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::PITCH_DOWN, deltaTime);
        else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
            cam.ProcessKeyboard(engine::BACKWARD, deltaTime);

    }


    void mouse_callback(double xposIn, double yposIn)
    {
        UNREFERENCED_PARAMETER(xposIn);
        UNREFERENCED_PARAMETER(yposIn);

        //engine::App::mouse_callback(win, xposIn, yposIn);

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
        engine::App::scroll_callback(xoffset, yoffset);

        cam.ProcessMouseScroll(static_cast<float>(yoffset));
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

    void updateUI(engine::Shader& shader) override
    {
        UNREFERENCED_PARAMETER(shader);

        drawUI();
    }

    void clean() override
    {
        // clean up any resources
        ourCube1.clean();
        //ourCube2.clean();
        ourPlane.clean();
    }

private:
    bool firstMouse = true;

    float lastX = 0.0f;
    float lastY = 0.0f;

    // camera
    engine::Camera cam{ glm::vec3(0.0f, 0.0f, 3.0f), true };


    engine::PointLight myPointLight{ 0 };
    engine::DirectionalLight myDirectionalLight{ 0 };
    engine::SpotLight mySpotLight{ 0 };

    engine::Model cushionModel;

    engine::Cube ourCube1;
    //engine::Cube ourCube2;
    engine::Plane ourPlane;

    engine::Text ourText;

    float rotation = 0.0f;


    void drawScene(engine::Shader& shader)
    {
        shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();

        // setup lights
        //myPointLight.draw(shader, projection, view, 3.0f, getLightPosition());
        //myDirectionalLight.draw(shader, projection, view, 1.0f, getLightPosition(), getLightTarget());
        mySpotLight.draw(shader, projection, view, 2.0f, getLightPosition(), getLightTarget());

        // activate phong shader
        shader.use();
        shader.setVec3("viewPos", cam.Position);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setInt("blinn", true);


        // render the loaded model
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(0.0f, -0.15f, 0.0f)); // translate it down so it's at the center of the scene
        model1 = glm::scale(model1, glm::vec3(0.3f));	// it's a bit too big for our scene, so scale it down
        model1 = glm::rotate(model1, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model1);
        cushionModel.draw(shader);



        // render test cube
        //ourCube1.draw(shader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        //ourCube2.draw(shader, glm::vec3(-1.5f, 0.0f, -1.0f), glm::vec3(0.15f, 0.15f, 0.15f), rotation, glm::vec3(1.0f, 1.0f, 0.0f));

        rotation += deltaTime * 10.0f;

        // render test plane
        ourPlane.draw(shader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
