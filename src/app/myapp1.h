#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"


class MyApp1 : public engine::App
{
public:
    MyApp1(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen)
    {
        // my application specific state gets initialized here

        lastX = width / 2.0f;
        lastY = height / 2.0f;

        init();
    }

    void init() override
    {
        // load models
        backpackModel = engine::Model("models/backpack/backpack.obj");
        cushionModel = engine::Model("models/cushion/cushion.obj");

        myPointLight.setup();
        myDirectionalLight1.setup();
        myDirectionalLight2.setup();
        mySpotLight.setup();


        ourCube.setup(engine::Material(engine::color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png"));
        ourPlane.setup(engine::Material(engine::color(0.1f), "textures/rusted_metal_diffuse.jpg", "textures/rusted_metal_specular.jpg"));
        ourBillboard.setup(engine::Material(engine::color(0.1f), "textures/grass.png"));

        ourText.setup();

        ourSkybox.setup();
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void keyCallback(GLFWwindow* win)
    {
        engine::App::key_callback(win);

        // Detect Shift key state
        bool shiftPressed = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;


        if (shiftPressed && glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::YAW_DOWN, deltaTime);
        else if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::LEFT, deltaTime);


        if (shiftPressed && glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::YAW_UP, deltaTime);
        else if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::RIGHT, deltaTime);


        if (shiftPressed && glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::PITCH_UP, deltaTime);
        else if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::FORWARD, deltaTime);


        if (shiftPressed && glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::PITCH_DOWN, deltaTime);
        else if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::BACKWARD, deltaTime);


        if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::UP, deltaTime);
        if (glfwGetKey(win, GLFW_KEY_Q) == GLFW_PRESS)
            cam.ProcessKeyboard(engine::DOWN, deltaTime);
    }


    void mouse_callback(GLFWwindow* win, double xposIn, double yposIn)
    {
        engine::App::mouse_callback(win, xposIn, yposIn);

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

        cam.ProcessMouseMovement(xoffset, yoffset);
    }

    void scroll_callback(GLFWwindow* win, double xoffset, double yoffset)
    {
        engine::App::scroll_callback(win, xoffset, yoffset);

        cam.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    void framebuffer_size_callback(GLFWwindow* win, int newWidth, int newHeight)
    {
        engine::App::framebuffer_size_callback(win, newWidth, newHeight);
    }


    void update() override
    {
        // draw scene and UI in framebuffer
        drawScene();
        drawUI();
    }

    void clean() override
    {
        // clean up any resources
        ourSkybox.clean();
        ourCube.clean();
        ourPlane.clean();
        ourBillboard.clean();
    }

private:
    bool firstMouse = true;

    float lastX = 0.0f;
    float lastY = 0.0f;

    // camera
    engine::Camera cam{ glm::vec3(0.0f, 0.0f, 3.0f), true };

    engine::Model backpackModel;
    engine::Model cushionModel;


    engine::PointLight myPointLight{ 0 };
    engine::DirectionalLight myDirectionalLight1{ 0 };
    engine::DirectionalLight myDirectionalLight2{ 1 };
    engine::SpotLight mySpotLight{ 0 };


    engine::Cube ourCube;
    engine::Plane ourPlane;
    engine::Billboard ourBillboard;

    engine::Text ourText;

    engine::Skybox ourSkybox;




    void drawScene()
    {
        phongShader.use();
    
    
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
    
    
    
    
        // setup lights
        //ourLights.Draw(lightingShader, projection, view);
        myPointLight.draw(phongShader, projection, view, 1.0f, glm::vec3(0.0f, 0.3f, 2.0f));
        myDirectionalLight1.draw(phongShader, projection, view, 1.0f, glm::vec3(2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        myDirectionalLight2.draw(phongShader, projection, view, 0.2f, glm::vec3(-2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        //mySpotLight.draw(lightingShader, projection, view, 1.0f, cam.Position, cam.Front);
        //mySpotLight.draw(lightingShader, projection, view, 1.0f, glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    
    
    
        // activate phong shader
        phongShader.use();
        phongShader.setVec3("viewPos", cam.Position);
        phongShader.setMat4("projection", projection);
        phongShader.setMat4("view", view);
    
    
    
    
    
        // render the loaded model
        //glm::mat4 model1 = glm::mat4(1.0f);
        //model1 = glm::translate(model1, glm::vec3(0.0f, -0.2f, 0.0f)); // translate it down so it's at the center of the scene
        //model1 = glm::scale(model1, glm::vec3(0.3f));	// it's a bit too big for our scene, so scale it down
        //model1 = glm::rotate(model1, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //phongShader.setMat4("model", model1);
        //cushionModel.draw(phongShader);
    
    
        // activate skybox reflection shader
        skyboxReflectShader.use();
        skyboxReflectShader.setMat4("view", view);
        skyboxReflectShader.setMat4("projection", projection);
        skyboxReflectShader.setVec3("cameraPos", cam.Position);
    
        // render the loaded model
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model2 = glm::scale(model2, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        skyboxReflectShader.setMat4("model", model2);
        backpackModel.draw(skyboxReflectShader);
    
    
    
    
    
        // render test cube
        ourCube.draw(phongShader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    
        ourBillboard.draw(phongShader, glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
        // render test plane
        ourPlane.draw(phongShader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
    
        ourSkybox.draw(projection, view);
    }
    
    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
    
    void cleanScene()
    {
        ourSkybox.clean();
    
        ourCube.clean();
        ourPlane.clean();
        ourBillboard.clean();
    }
};
