#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp3 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    // camera
    engine::Camera cam{ glm::vec3(0.0f, 0.0f, 3.0f), false };



    engine::Plane ourPlane{};
    engine::Sphere rustedIronSphere{};
    engine::Sphere goldSphere{};
    engine::Sphere grassSphere{};
    engine::Sphere plasticSphere{};
    engine::Sphere wallSphere{};



    engine::Text ourText{};


    engine::SpotLight mySpotLight{ 0 };


    // lights
    // ------
    glm::vec3 lightPositions[4] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 lightColors[4] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    unsigned int hdrTexture{};

    float rotation{};


public:
    MyApp3(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
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

        mySpotLight.setup(engine::Color{ 0.1f, 0.1f, 0.1f, 1.0f });
        mySpotLight.setCutOff(8.0f);
        mySpotLight.setOuterCutOff(20.f);

        // load PBR material textures
        // --------------------------
        rustedIronSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/rusted_iron/albedo.png",
            "",
            "textures/pbr/rusted_iron/normal.png",
            "textures/pbr/rusted_iron/metallic.png",
            "textures/pbr/rusted_iron/roughness.png",
            "textures/pbr/rusted_iron/ao.png"));

        goldSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/gold/albedo.png",
            "",
            "textures/pbr/gold/normal.png",
            "textures/pbr/gold/metallic.png",
            "textures/pbr/gold/roughness.png",
            "textures/pbr/gold/ao.png"));

        grassSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/grass/albedo.png",
            "",
            "textures/pbr/grass/normal.png",
            "textures/pbr/grass/metallic.png",
            "textures/pbr/grass/roughness.png",
            "textures/pbr/grass/ao.png"));

        plasticSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/plastic/albedo.png",
            "",
            "textures/pbr/plastic/normal.png",
            "textures/pbr/plastic/metallic.png",
            "textures/pbr/plastic/roughness.png",
            "textures/pbr/plastic/ao.png"));

        wallSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/wall/albedo.png",
            "",
            "textures/pbr/wall/normal.png",
            "textures/pbr/wall/metallic.png",
            "textures/pbr/wall/roughness.png",
            "textures/pbr/wall/ao.png"));

        hdrTexture = engine::Texture::loadImage("textures/hdr/newport_loft.hdr");



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

        //engine::App::mouse_callback(xposIn, yposIn);

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

    void update(engine::Shader& shader1, engine::Shader& shader2) override
    {
        // draw scene and UI in framebuffer
        drawScene(shader1, shader2);
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
        ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader1, engine::Shader& shader2)
    {
        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ cam.GetViewMatrix() };

        // activate pbr shader
        shader2.use();
        shader2.setMat4("projection", projection);
        shader2.setMat4("view", view);
        shader2.setVec3("camPos", cam.Position);
        shader2.setVec3("lightPositions[0]", glm::vec3(0.0f, 0.0f, 10.0f));
        shader2.setVec3("lightColors[0]", glm::vec3(255.0f, 255.0f, 255.0f));

        backgroundShader.use();
        backgroundShader.setMat4("projection", projection);

        // render test sphere
        rustedIronSphere.draw(shader2, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        goldSphere.draw(shader2, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        grassSphere.draw(shader2, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        plasticSphere.draw(shader2, glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        wallSphere.draw(shader2, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        

        rotation += deltaTime * 10.0f;

        
        shader1.use();
        shader1.setVec3("viewPos", cam.Position);
        shader1.setMat4("projection", projection);
        shader1.setMat4("view", view);
        shader1.setInt("blinn", true);


        // render light
        mySpotLight.draw(shader1, projection, view, 2.0f, getLightPosition(), getLightTarget());

        // render test plane
        ourPlane.draw(shader1, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

        // render skybox (render as last to prevent overdraw)
        backgroundShader.use();

        backgroundShader.setMat4("view", view);
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
        //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
