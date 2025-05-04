#pragma once

#include "app.h"
#include "../engine.h"

#include "../misc/noncopyable.h"
#include "../tools/file_system.h"



#include "../debug/imgui_debug.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace engine
{
    enum class RenderMethod
    {
        BlinnPhong = 0, // legacy
        PBR = 1 // mainstream
    };

    struct SceneSettings
    {
        RenderMethod method{};
        
        bool HDRSkyboxHide{ false };
        std::string HDRSkyboxFilePath{};
        float HDRSkyboxBlurStrength{ 0.0f };

        float shadowIntensity{ 1.5f };
        float iblDiffuseIntensity{ 1.0f };
        float iblSpecularIntensity{ 1.0f };

        bool applyGammaCorrection{};
    };
    
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class Scene : private NonCopyable
    {
    private:
        bool key_w_pressed { false };

        const unsigned int SHADOW_WIDTH{ 2048 }, SHADOW_HEIGHT{ 2048 };

        unsigned int depthMapFramebuffer{};
        unsigned int colorFramebuffer{};



        unsigned int rbo{}; // renderbuffer object



        unsigned int textureDepthMapBuffer{};
        unsigned int textureColorbuffer{};


        Shader screenShader{};
        Shader simpleDepthShader{};
        Shader debugDepthQuad{};

        
        ImGuiDebug m_debug{};

        GLuint query{};

        void before_init_internal();
        void after_init_internal();

        void setup_BlinnPhong();
        void setupPBR();

        void loop_BlinnPhong();
        void loop_PBR();


    
    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};

        GLint polycount{};
        GLint meshcount{};

        // settings
        std::string title{};

        App* app{};
        SceneSettings settings{};

        virtual void before_init_hook() {}; // Overridable by derived classes
        virtual void after_init_hook() {}; // Overridable by derived classes

    public:
        bool show_window{ false };

        // default camera
        engine::Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), true };


        std::shared_ptr<Entity> rootEntity{};

        std::vector<std::shared_ptr<engine::Light>> lights{};

        Shader blinnPhongShader{};
        Shader pbrShader{};
        Shader skyboxReflectShader{};
        Shader backgroundShader{};

        // PBR
        Shader equirectangularToCubemapShader{};
        Shader irradianceShader{};
        Shader prefilterShader{};
        Shader brdfShader{};

 


        unsigned int irradianceMap{};
        unsigned int prefilterMap{};
        unsigned int brdfLUTTexture{};
        unsigned int envCubemap{};


        

        Scene(std::string _title, App* _app, SceneSettings _settings);

        void initialize();


        // must be overridden in derived class
        virtual void init() = 0;

        void before_init() {
            before_init_internal();     // Always called
            before_init_hook();         // Hook for derived logic
        }

        void after_init() {
            after_init_internal();     // Always called
            after_init_hook();         // Hook for derived logic
        }

        // must be overridden in derived class
        virtual void update(Shader& shader) = 0;

        // must be overridden in derived class
        virtual void updateUI() = 0;

        // must be overridden in derived class
        virtual void clean() = 0;


        void gameLoop();

        void exit();

        GLFWwindow* getWindow()
        {
            return app->window;
        }

        


        void key_callback(int key, int scancode, int action, int mods)
        {
            UNREFERENCED_PARAMETER(scancode);
            UNREFERENCED_PARAMETER(mods);
            
            // basic window handling
            switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(app->window, GL_TRUE); break;
            case GLFW_KEY_ENTER:
                if (action == GLFW_RELEASE)
                {
                    app->toggleFullscreen([this]() {
                        this->refreshFullscreen();
                        });
                }
                break;
            }

            if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_PRESS)
            {
                if (!key_w_pressed) // Only toggle when the key is first pressed
                {
                    show_window = !show_window;
                    key_w_pressed = true; // Mark the key as pressed
                }
            }
            else if (glfwGetKey(app->window, GLFW_KEY_W) == GLFW_RELEASE)
            {
                key_w_pressed = false; // Reset the state when the key is released
            }
        }

        // glfw: whenever the mouse moves, this callback is called
        // -------------------------------------------------------
        void mouse_callback(double xposIn, double yposIn)
        {
            if (show_window)
                ImGui_ImplGlfw_CursorPosCallback(app->window, xposIn, yposIn);
        }

        // glfw: whenever the mouse scroll wheel scrolls, this callback is called
        // ----------------------------------------------------------------------
        void scroll_callback(double xoffset, double yoffset)
        {
            UNREFERENCED_PARAMETER(xoffset);
            UNREFERENCED_PARAMETER(yoffset);
        }

        // https://github.com/SonarSystems/OpenGL-Tutorials/blob/master/GLFW%20Joystick%20Input/main.cpp
        void gamepad_callback(const GLFWgamepadstate& state)
        {
            UNREFERENCED_PARAMETER(state);
        }

        // glfw: whenever the window size changed (by OS or user resize) this callback function executes
        // ---------------------------------------------------------------------------------------------
        void framebuffer_size_callback(int newWidth, int newHeight)
        {
            // make sure the viewport matches the new window dimensions; note that width and 
            // height will be significantly larger than specified on retina displays.
            glViewport(0, 0, newWidth, newHeight);
        }

        void refreshFullscreen()
        {
            // reinit framebuffers because width and height changed
            //initDepthMapFramebuffer();
            initColorFramebuffer();
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
        unsigned int quadVBO = 0;
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




    private:
        static void glfw_error_callback(int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
            std::exit(EXIT_FAILURE);
        }

        void enableDepthTest(bool enable)
        {
            // enable z buffer (depth test) to have correct objects depth ordering
            if (enable)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
        }

        void enableFaceCulling(bool enable)
        {
            if (enable)
            {
                // optim : do not display hidden faces
                // consistent winding orders needed (counter-clockwise by default)
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                glFrontFace(GL_CCW);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }
        }

        void enableAntiAliasing(bool enable)
        {
            if (enable)
            {
                // MSAA anti aliasing
                glfwWindowHint(GLFW_SAMPLES, 4);
                glEnable(GL_MULTISAMPLE);
            }
        }

        void enableGammaCorrection(bool enable)
        {
            // gamma correction (default 2.2 gamma correction)
            if (enable)
                glEnable(GL_FRAMEBUFFER_SRGB);
            else
                glDisable(GL_FRAMEBUFFER_SRGB);
        }

        void initDepthMapFramebuffer()
        {
            // create depth framebuffer
            glGenFramebuffers(1, &depthMapFramebuffer);
            // create depth texture
            glGenTextures(1, &textureDepthMapBuffer);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            // attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthMapBuffer, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // shader configuration
            // --------------------
            debugDepthQuad.use();
            debugDepthQuad.setInt("depthMap", 0);
        }

        void computeDepthMapFramebuffer(Shader& shader, std::shared_ptr<Light> light)
        {
            glm::vec3 light_position = light->getPosition();
            glm::vec3 light_target = light->getTarget();

            // 1. render depth of scene to texture (from light's perspective)
            // --------------------------------------------------------------
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 0.1f;  // Previously 1.0f
            float far_plane = 100.0f;  // Previously 7.5f
            lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
            lightView = glm::lookAt(light_position, light_target, glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            // render scene from light's point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
            glClear(GL_DEPTH_BUFFER_BIT);


            glEnable(GL_POLYGON_OFFSET_FILL); // fix peter panning
            glPolygonOffset(2.0f, 4.0f); // Adjust these values to fine-tune shadow biasing
            update(simpleDepthShader);
            glDisable(GL_POLYGON_OFFSET_FILL);
           

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // reset viewport
            glViewport(0, 0, app->width, app->height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 2. render scene as normal using the previously generated depth/shadow map  
            // -------------------------------------------------------------------------
            shader.use();
            shader.setVec3("lightPos", light_position); // ?????????????????
            shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            // update user stuffs
            update(shader);

            glActiveTexture(GL_TEXTURE10);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
            shader.setInt("material.texture_shadowMap", 10);

            // 3. render Depth map to quad
            // ---------------------------
            debugDepthQuad.use();
            debugDepthQuad.setFloat("near_plane", near_plane);
            debugDepthQuad.setFloat("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);

            // test depth map (also comment computeColorFramebuffer);
            //renderQuad();
        }

        void initColorFramebuffer()
        {
            // create framebuffer
            glGenFramebuffers(1, &colorFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
            // create a color attachment texture
            glGenTextures(1, &textureColorbuffer);
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, app->width, app->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
            // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, app->width, app->height); // use a single renderbuffer object for both a depth AND stencil buffer.
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
            // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void computeColorFramebuffer()
        {
            // draw color framebuffer to screen
            // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
            // clear all relevant buffers
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // set clear color (not really necessary actually, since we won't be able to see behind the quad anyways)
            glClear(GL_COLOR_BUFFER_BIT);

            screenShader.use();
            
            //glBindVertexArray(quadVAO);
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
            glDrawArrays(GL_TRIANGLES, 0, 6);
            renderQuad();
        }

        void loadShaders()
        {
            // blinn phong illumination model and lightning shader
            blinnPhongShader.init("blinnphong", "shaders/blinn-phong.vertex", "shaders/blinn-phong.frag");

            pbrShader.init("pbr", "shaders/pbr.vertex", "shaders/pbr.frag");

            //Shader depthBufferShader("debug_depth_buffer", "shaders/debug/debug_depth_buffer.vertex", "shaders/debug/debug_depth_buffer.frag"); // depth buffer debugging shader

            // color framebuffer to screen shader
            screenShader.init("screen", "shaders/framebuffers_screen.vertex", "shaders/framebuffers_screen.frag");

            // skybox reflection shader
            skyboxReflectShader.init("cubemap", "shaders/cubemap.vertex", "shaders/cubemap.frag");

            simpleDepthShader.init("simpleDepthBuffer", "shaders/shadow_mapping_depth.vertex", "shaders/shadow_mapping_depth.frag");
            debugDepthQuad.init("debugDepthQuad", "shaders/debug/debug_quad_depth.vertex", "shaders/debug/debug_quad_depth.frag");

            

			// PBR
            equirectangularToCubemapShader.init("equirectangularToCubemapShader", "shaders/cubemap2.vertex", "shaders/equirectangular_to_cubemap.frag");
            irradianceShader.init("irradianceShader", "shaders/cubemap2.vertex", "shaders/irradiance_convolution.frag");
            prefilterShader.init("prefilterShader", "shaders/cubemap2.vertex", "shaders/prefilter.frag");
            brdfShader.init("brdfShader", "shaders/brdf.vertex", "shaders/brdf.frag");

            backgroundShader.init("background", "shaders/background.vertex", "shaders/background.frag");
        }

        // Function to count vertices and polygons
        void beginQuery()
        {
            glGenQueries(1, &query);
            glBeginQuery(GL_PRIMITIVES_GENERATED, query);
        }

        // Function to count vertices and polygons
        void endQuery()
        {
            glEndQuery(GL_PRIMITIVES_GENERATED);
            
            glGetQueryObjectiv(query, GL_QUERY_RESULT, &polycount);

            glDeleteQueries(1, &query);
        }



    };
}