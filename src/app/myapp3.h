#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

class MyApp3 : public engine::App
{
private:
    bool firstMouse{ true };

    float lastX{ 0.0f };
    float lastY{ 0.0f };


    engine::Sphere rustedIronSphere{};
    engine::Sphere goldSphere{};
    engine::Sphere grassSphere{};
    engine::Sphere plasticSphere{};
    engine::Sphere wallSphere{};



    engine::Text ourText{};


    engine::SpotLight mySpotLight{ 0 };

    
    // rusted iron
    //unsigned int ironAlbedoMap = engine::Texture::loadTexture("textures/pbr/rusted_iron/albedo.png");
    //unsigned int ironNormalMap = engine::Texture::loadTexture("textures/pbr/rusted_iron/normal.png");
    //unsigned int ironMetallicMap = engine::Texture::loadTexture("textures/pbr/rusted_iron/metallic.png");
    //unsigned int ironRoughnessMap = engine::Texture::loadTexture("textures/pbr/rusted_iron/roughness.png");
    //unsigned int ironAOMap = engine::Texture::loadTexture("textures/pbr/rusted_iron/ao.png");

    //// gold
    //unsigned int goldAlbedoMap = engine::Texture::loadTexture("textures/pbr/gold/albedo.png");
    //unsigned int goldNormalMap = engine::Texture::loadTexture("textures/pbr/gold/normal.png");
    //unsigned int goldMetallicMap = engine::Texture::loadTexture("textures/pbr/gold/metallic.png");
    //unsigned int goldRoughnessMap = engine::Texture::loadTexture("textures/pbr/gold/roughness.png");
    //unsigned int goldAOMap = engine::Texture::loadTexture("textures/pbr/gold/ao.png");

    //// grass
    //unsigned int grassAlbedoMap = engine::Texture::loadTexture("textures/pbr/grass/albedo.png");
    //unsigned int grassNormalMap = engine::Texture::loadTexture("textures/pbr/grass/normal.png");
    //unsigned int grassMetallicMap = engine::Texture::loadTexture("textures/pbr/grass/metallic.png");
    //unsigned int grassRoughnessMap = engine::Texture::loadTexture("textures/pbr/grass/roughness.png");
    //unsigned int grassAOMap = engine::Texture::loadTexture("textures/pbr/grass/ao.png");

    //// plastic
    //unsigned int plasticAlbedoMap = engine::Texture::loadTexture("textures/pbr/plastic/albedo.png");
    //unsigned int plasticNormalMap = engine::Texture::loadTexture("textures/pbr/plastic/normal.png");
    //unsigned int plasticMetallicMap = engine::Texture::loadTexture("textures/pbr/plastic/metallic.png");
    //unsigned int plasticRoughnessMap = engine::Texture::loadTexture("textures/pbr/plastic/roughness.png");
    //unsigned int plasticAOMap = engine::Texture::loadTexture("textures/pbr/plastic/ao.png");

    //// wall
    //unsigned int wallAlbedoMap = engine::Texture::loadTexture("textures/pbr/wall/albedo.png");
    //unsigned int wallNormalMap = engine::Texture::loadTexture("textures/pbr/wall/normal.png");
    //unsigned int wallMetallicMap = engine::Texture::loadTexture("textures/pbr/wall/metallic.png");
    //unsigned int wallRoughnessMap = engine::Texture::loadTexture("textures/pbr/wall/roughness.png");
    //unsigned int wallAOMap = engine::Texture::loadTexture("textures/pbr/wall/ao.png");



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

        // override default camera properties
        camera.Position = glm::vec3(0.0f, 0.0f, 2.0f);
        camera.Fps = false;

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


        ourText.setup(width, height);



        // view/projection transformations
        glm::mat4 projection{ glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        glm::mat4 view{ camera.GetViewMatrix() };

        // initialize static shader uniforms before rendering
        // --------------------------------------------------
        backgroundShader.use();
        backgroundShader.setMat4("projection", projection);

        // then before rendering, configure the viewport to the original framebuffer's screen dimensions
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
        glViewport(0, 0, scrWidth, scrHeight);
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
        //ourPlane.clean();
    }

private:
    void drawScene(engine::Shader& shader1, engine::Shader& shader2)
    {
        // view/projection transformations
        //glm::mat4 projection{ glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        //glm::mat4 view{ cam.GetViewMatrix() };

        //// activate pbr shader
        //shader2.use();
        //shader2.setMat4("projection", projection);
        //shader2.setMat4("view", view);
        //shader2.setVec3("camPos", cam.Position);
        ////shader2.setVec3("lightPositions[0]", glm::vec3(0.0f, 0.0f, 10.0f));
        ////shader2.setVec3("lightColors[0]", glm::vec3(255.0f, 255.0f, 255.0f));

        //backgroundShader.use();
        //backgroundShader.setMat4("projection", projection);


        

        //rotation += deltaTime * 10.0f;

        //
        ////shader1.use();
        ////shader1.setVec3("viewPos", cam.Position);
        ////shader1.setMat4("projection", projection);
        ////shader1.setMat4("view", view);
        ////shader1.setInt("blinn", true);


        //// render light
        /////mySpotLight.draw(shader1, projection, view, 2.0f, getLightPosition(), getLightTarget());

        //// render test plane
        ////ourPlane.draw(shader1, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));


        //// render light source (simply re-render sphere at light positions)
        //// this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        //// keeps the codeprint small.
        //for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        //{
        //    glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        //    newPos = lightPositions[i];
        //    shader2.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        //    shader2.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

        //    //model = glm::mat4(1.0f);
        //    //model = glm::translate(model, newPos);
        //    //model = glm::scale(model, glm::vec3(0.5f));
        //    //pbrShader.setMat4("model", model);
        //    //pbrShader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //    rustedIronSphere.draw(shader2, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //    goldSphere.draw(shader2, glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //    grassSphere.draw(shader2, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //    plasticSphere.draw(shader2, glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //    wallSphere.draw(shader2, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        //}


        //// render skybox (render as last to prevent overdraw)
        //backgroundShader.use();

        //backgroundShader.setMat4("view", view);

        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        //glm::mat4 projection{ glm::perspective(glm::radians(cam.Zoom), (float)width / (float)height, 0.1f, 100.0f) };
        //glm::mat4 view{ cam.GetViewMatrix() };






        // render scene, supplying the convoluted irradiance map to the final shader.
        // ------------------------------------------------------------------------------------------
        glm::mat4 model = glm::mat4(1.0f);


        // bind pre-computed IBL data
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);



        // render test sphere
        rustedIronSphere.draw(shader2, glm::vec3(-5.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        goldSphere.draw(shader2, glm::vec3(-3.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        grassSphere.draw(shader2, glm::vec3(-1.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        plasticSphere.draw(shader2, glm::vec3(1.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));
        wallSphere.draw(shader2, glm::vec3(3.0f, 0.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));





        //// rusted iron
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, ironAlbedoMap);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, ironNormalMap);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, ironMetallicMap);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, ironRoughnessMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, ironAOMap);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(-5.0, 0.0, 2.0));
        //shader2.setMat4("model", model);
        //shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //renderSphere();

        //// gold
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, goldAlbedoMap);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, goldNormalMap);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, goldMetallicMap);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, goldRoughnessMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, goldAOMap);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
        //shader2.setMat4("model", model);
        //shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //renderSphere();

        //// grass
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, grassAlbedoMap);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, grassNormalMap);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, grassMetallicMap);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, grassRoughnessMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, grassAOMap);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(-1.0, 0.0, 2.0));
        //shader2.setMat4("model", model);
        //shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //renderSphere();

        //// plastic
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, plasticAlbedoMap);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, plasticNormalMap);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, plasticMetallicMap);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, plasticRoughnessMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, plasticAOMap);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(1.0, 0.0, 2.0));
        //shader2.setMat4("model", model);
        //shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //renderSphere();

        //// wall
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, wallAlbedoMap);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, wallNormalMap);
        //glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, wallMetallicMap);
        //glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, wallRoughnessMap);
        //glActiveTexture(GL_TEXTURE4);
        //glBindTexture(GL_TEXTURE_2D, wallAOMap);

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
        //shader2.setMat4("model", model);
        //shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //renderSphere();

        // render light source (simply re-render sphere at light positions)
        // this looks a bit off as we use the same shader, but it'll make their positions obvious and 
        // keeps the codeprint small.
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
        {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
            newPos = lightPositions[i];
            shader2.setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
            shader2.setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            shader2.setMat4("model", model);
            shader2.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderSphere();
        }
    }

    void drawUI()
    {
        // render HUD / UI
        ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }

    // renders (and builds at first invocation) a sphere
    // -------------------------------------------------
    unsigned int sphereVAO = 0;
    GLsizei indexCount;
    void renderSphere()
    {
        if (sphereVAO == 0)
        {
            glGenVertexArrays(1, &sphereVAO);

            unsigned int vbo, ebo;
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &ebo);

            std::vector<glm::vec3> positions;
            std::vector<glm::vec2> uv;
            std::vector<glm::vec3> normals;
            std::vector<unsigned int> indices;

            const unsigned int X_SEGMENTS = 64;
            const unsigned int Y_SEGMENTS = 64;
            const float PI = 3.14159265359f;
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
                {
                    float xSegment = (float)x / (float)X_SEGMENTS;
                    float ySegment = (float)y / (float)Y_SEGMENTS;
                    float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                    float yPos = std::cos(ySegment * PI);
                    float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                    positions.push_back(glm::vec3(xPos, yPos, zPos));
                    uv.push_back(glm::vec2(xSegment, ySegment));
                    normals.push_back(glm::vec3(xPos, yPos, zPos));
                }
            }

            bool oddRow = false;
            for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
            {
                if (!oddRow) // even rows: y == 0, y == 2; and so on
                {
                    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                    {
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    }
                }
                else
                {
                    for (int x = X_SEGMENTS; x >= 0; --x)
                    {
                        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                        indices.push_back(y * (X_SEGMENTS + 1) + x);
                    }
                }
                oddRow = !oddRow;
            }
            indexCount = static_cast<GLsizei>(indices.size());

            std::vector<float> data;
            for (unsigned int i = 0; i < positions.size(); ++i)
            {
                data.push_back(positions[i].x);
                data.push_back(positions[i].y);
                data.push_back(positions[i].z);
                if (normals.size() > 0)
                {
                    data.push_back(normals[i].x);
                    data.push_back(normals[i].y);
                    data.push_back(normals[i].z);
                }
                if (uv.size() > 0)
                {
                    data.push_back(uv[i].x);
                    data.push_back(uv[i].y);
                }
            }
            glBindVertexArray(sphereVAO);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
            unsigned int stride = (3 + 2 + 3) * sizeof(float);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        }

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    }

    // renderCube() renders a 1x1 3D cube in NDC.
        // -------------------------------------------------
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;
    void renderCube()
    {
        // initialize (if necessary)
        if (cubeVAO == 0)
        {
            float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                 // bottom face
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 // top face
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
            };
            glGenVertexArrays(1, &cubeVAO);
            glGenBuffers(1, &cubeVBO);
            // fill buffer
            glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // link vertex attributes
            glBindVertexArray(cubeVAO);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        // render Cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    // renderQuad() renders a 1x1 XY quad in NDC
    // -----------------------------------------
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }



};
