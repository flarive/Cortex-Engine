#pragma once

#include "../engine.h"

#include "../misc/noncopyable.h"
#include "../file_system.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace engine
{
    enum class RenderMethod
    {
        BlinnPhong = 0, // legacy
        PBR = 1
    };
    
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App : private NonCopyable
    {
    private:
        const int TARGET_FPS{ 60 };
        const int FRAME_DELAY{ 1000 / TARGET_FPS }; // in milliseconds


        bool key_w_pressed { false };


        bool show_window{ false };



        const unsigned int SHADOW_WIDTH{ 2048 }, SHADOW_HEIGHT{ 2048 };

        unsigned int depthMapFramebuffer{};
        unsigned int colorFramebuffer{};


        //unsigned int quadVAO{}, quadVBO{};
        unsigned int rbo{}; // renderbuffer object



        unsigned int textureDepthMapBuffer{};
        unsigned int textureColorbuffer{};

        glm::vec3 m_lightPosition{};
        glm::vec3 m_lightTarget{};

        

        Shader screenShader{};
        Shader simpleDepthShader{};
        Shader debugDepthQuad{};

        


    
    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};

        // settings
        std::string title{};
        int width{}; // windowed width
        int height{}; // windowed height
        bool fullscreen{};
        RenderMethod method;


    public:
        GLFWwindow* window{};

        // default camera
        engine::Camera camera{ glm::vec3(0.0f, 0.0f, 3.0f), true };

        Shader blinnPhongShader{};
        Shader pbrShader{};
        Shader skyboxReflectShader{};
        Shader backgroundShader{};

        // PBR
        Shader equirectangularToCubemapShader{};
        Shader irradianceShader{};
        Shader prefilterShader{};
        Shader brdfShader{};


        unsigned int irradianceMap;
        unsigned int prefilterMap;
        unsigned int brdfLUTTexture;
        unsigned int envCubemap{};


        

        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, RenderMethod _method = RenderMethod::PBR)
            : title(_title), width(_width), height(_height), fullscreen(_fullscreen), method(_method)
        {
            if (_method == RenderMethod::PBR)
                setupPBR(); 
            else
                setup_BlinnPhong();
        }

        void setup_BlinnPhong()
        {
            initGLFW();

            const char* glsl_version = initOpenGL();

            initWindow();
            
            // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
            glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

            enableVerticalSync(true);

            enableMouseCapture(true);

            initGLAD();

            initImGUI(glsl_version);

            // configure global opengl state
            // -----------------------------
            enableDepthTest(true);
            enableFaceCulling(true);
            enableAntiAliasing(true);
            //enableGammaCorrection(true);


            loadShaders();


            // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
            // -------------------------------------------------------------------------------------------
            blinnPhongShader.use();
            blinnPhongShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
            blinnPhongShader.setFloat("material.shininess", 32.0f);

            // shader configuration
            // --------------------
            screenShader.use();
            screenShader.setInt("screenTexture", 0);

            // Depth map framebuffer configuration (for shadow map)
            // -----------------------------------
            initDepthMapFramebuffer();

            // color framebuffer configuration
            // -------------------------
            initColorFramebuffer();

            // uncomment this call to draw in wireframe polygons.
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE
        }


        void setupPBR()
        {
            initGLFW();

            const char* glsl_version = initOpenGL();

            initWindow();

            // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
            glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

            enableVerticalSync(true);

            enableMouseCapture(true);

            initGLAD();

            initImGUI(glsl_version);


            // configure global opengl state
            // -----------------------------
            enableDepthTest(true);
            // set depth function to less than AND equal for skybox depth trick.
            glDepthFunc(GL_LEQUAL);
            // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
            glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

            //enableFaceCulling(true);
            enableAntiAliasing(true);
            //enableGammaCorrection(true);

            // build and compile shaders
            // -------------------------
            loadShaders();


            // prepare screen quad VAO that will render the main framebuffer
            //glGenVertexArrays(1, &quadVAO);
            //glGenBuffers(1, &quadVBO);
            //glBindVertexArray(quadVAO);
            //glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            //glBufferData(GL_ARRAY_BUFFER, sizeof(engine::screenQuadVertices), &engine::screenQuadVertices, GL_STATIC_DRAW);
            //glEnableVertexAttribArray(0);
            //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            //glEnableVertexAttribArray(1);
            //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


            pbrShader.use();
            pbrShader.setInt("albedoMap", 0);
            pbrShader.setInt("normalMap", 1);
            pbrShader.setInt("metallicMap", 2);
            pbrShader.setInt("roughnessMap", 3);
            pbrShader.setInt("aoMap", 4);

            pbrShader.setInt("irradianceMap", 5);
            pbrShader.setInt("prefilterMap", 6);
            pbrShader.setInt("brdfLUT", 7);




            backgroundShader.use();
            backgroundShader.setInt("environmentMap", 0);


            // shader configuration
            // --------------------
            screenShader.use();
            screenShader.setInt("screenTexture", 0);

            // Depth map framebuffer configuration (for shadow map)
            // -----------------------------------
            //initDepthMapFramebuffer();

            // color framebuffer configuration
            // -------------------------
            initColorFramebuffer();



            int vsize = 512;

            // pbr: setup framebuffer
            // ----------------------
            unsigned int captureFBO;
            unsigned int captureRBO;
            glGenFramebuffers(1, &captureFBO);
            glGenRenderbuffers(1, &captureRBO);

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

            // pbr: load the HDR environment map
            // ---------------------------------
            unsigned int hdrTexture = engine::Texture::loadHDRImage(file_system::getPath("textures/hdr/photo_studio_loft_hall_2k.hdr"));

            // pbr: setup cubemap to render to and attach to framebuffer
            // ---------------------------------------------------------
            glGenTextures(1, &envCubemap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, vsize, vsize, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
            // ----------------------------------------------------------------------------------------------
            glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
            glm::mat4 captureViews[] =
            {
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
            };

            // pbr: convert HDR equirectangular environment map to cubemap equivalent
            // ----------------------------------------------------------------------
            equirectangularToCubemapShader.use();
            equirectangularToCubemapShader.setInt("equirectangularMap", 0);
            equirectangularToCubemapShader.setMat4("projection", captureProjection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);

            glViewport(0, 0, vsize, vsize); // don't forget to configure the viewport to the capture dimensions.
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            for (unsigned int i = 0; i < 6; ++i)
            {
                equirectangularToCubemapShader.setMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderCube();
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
            // --------------------------------------------------------------------------------
            glGenTextures(1, &irradianceMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

            // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
            // -----------------------------------------------------------------------------
            irradianceShader.use();
            irradianceShader.setInt("environmentMap", 0);
            irradianceShader.setMat4("projection", captureProjection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

            glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            for (unsigned int i = 0; i < 6; ++i)
            {
                irradianceShader.setMat4("view", captureViews[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                renderCube();
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
            // --------------------------------------------------------------------------------
            glGenTextures(1, &prefilterMap);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

            // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
            // ----------------------------------------------------------------------------------------------------
            prefilterShader.use();
            prefilterShader.setInt("environmentMap", 0);
            prefilterShader.setMat4("projection", captureProjection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            unsigned int maxMipLevels = 5;
            for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
            {
                // reisze framebuffer according to mip-level size.
                unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
                unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
                glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
                glViewport(0, 0, mipWidth, mipHeight);

                float roughness = (float)mip / (float)(maxMipLevels - 1);
                prefilterShader.setFloat("roughness", roughness);
                for (unsigned int i = 0; i < 6; ++i)
                {
                    prefilterShader.setMat4("view", captureViews[i]);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    renderCube();
                }
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // pbr: generate a 2D LUT from the BRDF equations used.
            // ----------------------------------------------------
            glGenTextures(1, &brdfLUTTexture);

            // pre-allocate enough memory for the LUT texture.
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, vsize, vsize, 0, GL_RG, GL_FLOAT, 0);
            // be sure to set wrapping mode to GL_CLAMP_TO_EDGE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
            glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
            glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

            glViewport(0, 0, vsize, vsize);
            brdfShader.use();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderQuad();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);



            // initialize static shader uniforms before rendering
            // --------------------------------------------------
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
            pbrShader.use();
            pbrShader.setMat4("projection", projection);
            backgroundShader.use();
            backgroundShader.setMat4("projection", projection);

            // then before rendering, configure the viewport to the original framebuffer's screen dimensions
            int scrWidth, scrHeight;
            glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
            glViewport(0, 0, scrWidth, scrHeight);
        }

        // must be overridden in derived class
        virtual void init() = 0;

        // must be overridden in derived class
        virtual void update(Shader& shader) = 0;

        // must be overridden in derived class
        virtual void updateUI() = 0;

        // must be overridden in derived class
        virtual void clean() = 0;

        void gameLoop()
        {
            while (!glfwWindowShouldClose(window))
            {
                // Poll and handle events (inputs, window resize, etc.)
                // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
                // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
                // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
                // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
                glfwPollEvents();
                if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
                {
                    ImGui_ImplGlfw_Sleep(10);
                    continue;
                }

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                framerate = ImGui::GetIO().Framerate;

                if (show_window)
                    renderUIWindow(show_window);

                float currentFrame = static_cast<float>(glfwGetTime());
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                

                auto start_time = std::chrono::high_resolution_clock::now();



                if (method == RenderMethod::PBR)
                    loop_PBR();
                else
                    loop_BlinnPhong();



                // ImGUI rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


                glfwSwapBuffers(window);

                auto end_time = std::chrono::high_resolution_clock::now();
                std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY) - (end_time - start_time));
            }

            glBindVertexArray(0);

            // optional: de-allocate all resources once they've outlived their purpose
            //glDeleteVertexArrays(1, &quadVAO);
            //glDeleteBuffers(1, &quadVBO);
            glDeleteRenderbuffers(1, &rbo);
            glDeleteFramebuffers(1, &colorFramebuffer);
            glDeleteFramebuffers(1, &depthMapFramebuffer);

            blinnPhongShader.clean();
            screenShader.clean();
            skyboxReflectShader.clean();

            // clean user stuffs
            clean();


            // imGui Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);

            glfwTerminate();
        }

        void loop_BlinnPhong()
        {
            // bind to color framebuffer and draw scene as we normally would to color texture 
            glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
            glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

            // make sure we clear the framebuffer's content
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // update user stuffs
            update(blinnPhongShader);

            // compute light shadows using a depth map framebuffer
            computeDepthMapFramebuffer();

            // render to framebuffer
            computeColorFramebuffer();

            // display UI/HUD above the scene and outside the framebuffer
            updateUI();
        }

        void loop_PBR()
        {
            // bind to color framebuffer and draw scene as we normally would to color texture 
            glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
            glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
            
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);



            pbrShader.use();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            pbrShader.setMat4("projection", projection);
            pbrShader.setMat4("view", view);
            pbrShader.setVec3("camPos", camera.Position);


            


            // bind pre-computed IBL data
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);


            // update user stuffs
            update(pbrShader);

            // render skybox (render as last to prevent overdraw)
            backgroundShader.use();
            backgroundShader.setMat4("view", view);
            backgroundShader.setMat4("projection", projection);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap); // display irradiance map
            //glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); // display prefilter map
            renderCube();

            // render BRDF map to screen
            //brdfShader.use();
            //renderQuad();

            // compute light shadows using a depth map framebuffer
            //computeDepthMapFramebuffer();

            // render to framebuffer
            computeColorFramebuffer();

            // display UI/HUD above the scene and outside the framebuffer
            updateUI();
        }

        void key_callback(int key, int scancode, int action, int mods)
        {
            UNREFERENCED_PARAMETER(scancode);
            UNREFERENCED_PARAMETER(mods);
            
            // basic window handling
            switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE); break;
            case GLFW_KEY_ENTER:
                if (action == GLFW_RELEASE) toggleFullscreen(); break;
            }

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                if (!key_w_pressed) // Only toggle when the key is first pressed
                {
                    show_window = !show_window;
                    key_w_pressed = true; // Mark the key as pressed
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
            {
                key_w_pressed = false; // Reset the state when the key is released
            }
        }

        // glfw: whenever the mouse moves, this callback is called
        // -------------------------------------------------------
        void mouse_callback(double xposIn, double yposIn)
        {
            UNREFERENCED_PARAMETER(xposIn);
            UNREFERENCED_PARAMETER(yposIn);
        }

        // glfw: whenever the mouse scroll wheel scrolls, this callback is called
        // ----------------------------------------------------------------------
        void scroll_callback(double xoffset, double yoffset)
        {
            UNREFERENCED_PARAMETER(xoffset);
            UNREFERENCED_PARAMETER(yoffset);
        }


        // glfw: whenever the window size changed (by OS or user resize) this callback function executes
        // ---------------------------------------------------------------------------------------------
        void framebuffer_size_callback(int newWidth, int newHeight)
        {
            // make sure the viewport matches the new window dimensions; note that width and 
            // height will be significantly larger than specified on retina displays.
            glViewport(0, 0, newWidth, newHeight);
        }

        void setLightPosition(glm::vec3 pos)
        {
            m_lightPosition = pos;
        }

        void setLightTarget(glm::vec3 pos)
        {
            m_lightTarget = pos;
        }

        glm::vec3 getLightPosition()
        {
            return m_lightPosition;
        }

        glm::vec3 getLightTarget()
        {
            return m_lightTarget;
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
            exit(EXIT_FAILURE);
        }

        void renderUIWindow(bool show)
        {
            ImGui::SetNextWindowSize(ImVec2(480, 60), ImGuiCond_Always);

            ImGui::Begin("Hello, world!", &show);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            ImGui::End();
        }

        void initGLFW()
        {
            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit())
            {
                std::cerr << "GLFW init failed" << std::endl;
                exit(EXIT_FAILURE);
            }

            glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA
        }

        const char* initOpenGL()
        {
            // GL 3.3 + GLSL 130
            const char* glsl_version = "#version 130";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
            //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

            return glsl_version;
        }

        void initWindow()
        {
            GLFWmonitor* myMonitor = glfwGetPrimaryMonitor(); // The primary monitor

            const GLFWvidmode* mode = glfwGetVideoMode(myMonitor);
            if (fullscreen)
            {
                width = mode->width;
                height = mode->height;
            }
            
            // Create window with graphics context
            window = glfwCreateWindow(width, height, "Learn OpenGL", fullscreen ? myMonitor : NULL, nullptr);
            if (window == NULL)
            {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                exit(EXIT_FAILURE);
            }

            glfwMakeContextCurrent(window);
        }

        void initGLAD()
        {
            // glad: load all OpenGL function pointers
            // ---------------------------------------
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        void initImGUI(const char* glsl_version)
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            if (io.ConfigFlags)
            {
                style.WindowRounding = 0.0f;
                style.ChildRounding = 5.0f;
                style.TabRounding = 5.f;
                style.FrameRounding = 5.f;
                style.GrabRounding = 5.f;
                style.PopupRounding = 5.f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;

                style.ItemSpacing.y = 8.0; // vertical padding between widgets
                style.FramePadding.x = 8.0; // better widget horizontal padding
                style.FramePadding.y = 4.0; // better widget vertical padding
            }

            // Apply Adobe Spectrum theme
            //https://github.com/adobe/imgui/blob/master/docs/Spectrum.md#imgui-spectrum
            ImGui::Spectrum::StyleColorsSpectrum();
            ImGui::Spectrum::LoadFont(17.0);


            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }

        void enableVerticalSync(bool enable)
        {
            // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
            glfwSwapInterval(enable ? 1 : 0);
        }

        void enableMouseCapture(bool enable)
        {
            // tell GLFW to capture our mouse
            if (!enable)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

        void computeDepthMapFramebuffer()
        {
            // 1. render depth of scene to texture (from light's perspective)
            // --------------------------------------------------------------
            glm::mat4 lightProjection, lightView;
            glm::mat4 lightSpaceMatrix;
            float near_plane = 0.1f;  // Previously 1.0f
            float far_plane = 20.0f;  // Previously 7.5f
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            lightView = glm::lookAt(m_lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
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
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 2. render scene as normal using the previously generated depth/shadow map  
            // -------------------------------------------------------------------------
            blinnPhongShader.use();
            blinnPhongShader.setVec3("lightPos", m_lightPosition);
            blinnPhongShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

            // update user stuffs
            update(blinnPhongShader);
            //update(pbrShader);

            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
            blinnPhongShader.setInt("material.texture_shadowMap", 6); // texture 6

            // 3. render Depth map to quad
            // ---------------------------
            debugDepthQuad.use();
            debugDepthQuad.setFloat("near_plane", near_plane);
            debugDepthQuad.setFloat("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
        }

        void initColorFramebuffer()
        {
            // create framebuffer
            glGenFramebuffers(1, &colorFramebuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
            // create a color attachment texture
            glGenTextures(1, &textureColorbuffer);
            glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
            // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
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

        // Toggle Fullscreen
        void toggleFullscreen()
        {
            static bool isFullscreen = false;

            // remember window original position and size
            static int windowPosX, windowPosY;
            static int windowWidth, windowHeight;

            if (!isFullscreen)
            {
                // Save window position and size
                glfwGetWindowPos(window, &windowPosX, &windowPosY);
                glfwGetWindowSize(window, &windowWidth, &windowHeight);

                // Get primary monitor
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                // Switch to fullscreen
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                glfwGetWindowSize(window, &width, &height);
            }
            else
            {
                // Restore windowed mode
                glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, 0);
                glfwGetWindowSize(window, &width, &height);
            }

            // reinit framebuffers because width and height changed
            initDepthMapFramebuffer();
            initColorFramebuffer();
            
            isFullscreen = !isFullscreen;
        }

        


        
    };
}