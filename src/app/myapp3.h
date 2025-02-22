#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp3 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };

    engine::Model redSciFiMetalSphere{};
    engine::Sphere rustedIronSphere{};
    engine::Sphere goldSphere{};
    engine::Sphere grassSphere{};
    engine::Sphere plasticSphere{};
    engine::Sphere wallSphere{};
    engine::Sphere bronzeSphere{};

    engine::Plane ourPlane{};

    engine::Text ourText{};


    engine::Shader lightCubeShader{};

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
        : engine::App(_title, _width, _height, _fullscreen, engine::RenderMethod::PBR)
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

        // override default camera properties
        camera.Position = glm::vec3(0.0f, 0.0f, 2.0f);
        camera.Fps = false;
        camera.Zoom = 75.0f;

        ourPlane.setup(engine::Material(engine::Color(0.1f), "textures/wood_diffuse.png", "textures/wood_specular.png"), engine::UvMapping(2.0f));


        lightCubeShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");


        redSciFiMetalSphere = engine::Model("models/sphere/smooth_sphere_80.obj");

        // load PBR material textures
        // --------------------------
        //redSciFiMetalSphere.setup(engine::Material(engine::Color(0.1f),
        //    "textures/pbr/red_scifi_metal/albedo.png",
        //    "",
        //    "textures/pbr/red_scifi_metal/normal.png",
        //    "textures/pbr/red_scifi_metal/metallic.png",
        //    "textures/pbr/red_scifi_metal/roughness.png",
        //    "textures/pbr/red_scifi_metal/ao.png",
        //    "textures/pbr/red_scifi_metal/height.png"), engine::UvMapping(3.0f));

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
            "textures/pbr/wall/ao.png"), engine::UvMapping(1.0f));

        bronzeSphere.setup(engine::Material(engine::Color(0.1f),
            "textures/pbr/bronze/albedo.png",
            "",
            "textures/pbr/bronze/normal.png",
            "textures/pbr/bronze/metallic.png",
            "textures/pbr/bronze/roughness.png",
            "textures/pbr/bronze/ao.png"));


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


        rotation += deltaTime * 10.0f;


        ourPlane.draw(shader, glm::vec3(0.0f, -10.50f, -10.0f), glm::vec3(12.0f, 12.0f, 12.0f), 180.0f, glm::vec3(1.0f, 0.0f, 0.0f));


        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };

        // add some custom light sources
        //render light source (simply re-render sphere at light positions)
        //this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        //keeps the codeprint small.
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 10.0, 0.0);
            newPos = lightPositions[i];

            shader.use();
            shader.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            shader.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            shader.setMat4("model", model);
            shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));


            // also draw the lamp object(s)
            lightCubeShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            lightCubeShader.setMat4("model", model);


            App::renderSphere();
        }

        
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
};
