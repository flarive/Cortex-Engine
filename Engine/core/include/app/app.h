#pragma once

#include "../engine.h"


#include <iostream>
#include <chrono>
#include <thread>

namespace engine
{
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App
    {
    public:
        GLFWwindow* window;

        engine::Shader phongShader;
        engine::Shader blinnPhongShader;
        engine::Shader screenShader;
        engine::Shader skyboxReflectShader;

        engine::Shader simpleDepthShader;
        engine::Shader debugDepthQuad;


        glm::vec3 lightPos;// = glm::vec3(-2.0f, 4.0f, -1.0f);


        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen)
            : title(_title), width(_width), height(_height), fullscreen(_fullscreen)
        {
            setup();
        }

        void key_callback(GLFWwindow* win)
        {
            // basic window handling
            if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                std::cout << "exiting..." << std::endl;
                glfwSetWindowShouldClose(window, GL_TRUE);
            }

            if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
            {
                if (!key_w_pressed) // Only toggle when the key is first pressed
                {
                    show_window = !show_window;
                    key_w_pressed = true; // Mark the key as pressed
                }
            }
            else if (glfwGetKey(win, GLFW_KEY_W) == GLFW_RELEASE)
            {
                key_w_pressed = false; // Reset the state when the key is released
            }
        }

        // glfw: whenever the mouse moves, this callback is called
        // -------------------------------------------------------
        void mouse_callback(GLFWwindow* win, double xposIn, double yposIn)
        {
            UNREFERENCED_PARAMETER(win);
            UNREFERENCED_PARAMETER(xposIn);
            UNREFERENCED_PARAMETER(yposIn);
        }

        // glfw: whenever the mouse scroll wheel scrolls, this callback is called
        // ----------------------------------------------------------------------
        void scroll_callback(GLFWwindow* win, double xoffset, double yoffset)
        {
            UNREFERENCED_PARAMETER(win);
            UNREFERENCED_PARAMETER(xoffset);
            UNREFERENCED_PARAMETER(yoffset);
        }

        // glfw: whenever the window size changed (by OS or user resize) this callback function executes
        // ---------------------------------------------------------------------------------------------
        static void framebuffer_size_callback(GLFWwindow* win, int width, int height)
        {
            UNREFERENCED_PARAMETER(win);

            // make sure the viewport matches the new window dimensions; note that width and 
            // height will be significantly larger than specified on retina displays.
            glViewport(0, 0, width, height);
        }

        void setup()
        {
            // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit())
            {
                //return 1;
                std::cerr << "GLFW init failed" << std::endl;
                exit(EXIT_FAILURE);
            }

            // GL 3.3 + GLSL 130
            const char* glsl_version = "#version 130";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
            //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


            GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor

            const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);
            if (fullscreen)
            {
                width = mode->width;
                height = mode->height;
            }

            // Create window with graphics context
            window = glfwCreateWindow(width, height, "Learn OpenGL", fullscreen ? MyMonitor : NULL, nullptr);
            if (window == NULL)
            {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                exit(EXIT_FAILURE);
            }


            glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

            glfwMakeContextCurrent(window);
            //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
            //glfwSetCursorPosCallback(window, mouse_callback);
            //glfwSetScrollCallback(window, scroll_callback);

            // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
            glfwSwapInterval(1);

            // tell GLFW to capture our mouse
            //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


            // glad: load all OpenGL function pointers
            // ---------------------------------------
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cerr << "Failed to initialize GLAD" << std::endl;
                exit(EXIT_FAILURE);
            }


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



            // configure global opengl state
            // -----------------------------
            // enable z buffer (depth test) to have correct objects depth ordering
            glEnable(GL_DEPTH_TEST);
            // optim : do not display hidden faces
            // consistent winding orders needed (counter-clockwise by default)
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);


            // anti aliasing MSAA
            glfwWindowHint(GLFW_SAMPLES, 4);
            glEnable(GL_MULTISAMPLE);

            // gamma correction
            //glEnable(GL_FRAMEBUFFER_SRGB);


            // load shaders

            // phong illimuniation model and lightning shader
            phongShader = engine::Shader("phong", "shaders/phong.vertex", "shaders/phong.frag");

            // phong illimuniation model and lightning shader
            blinnPhongShader = engine::Shader("phong", "shaders/blinn-phong.vertex", "shaders/blinn-phong.frag");

            //Shader depthBufferShader("depthbuffer", "shaders/depthbuffer.vertex", "shaders/depthbuffer.frag"); // depth buffer debugging shader

            // color framebuffer to screen shader
            screenShader = engine::Shader("screen", "shaders/framebuffers_screen.vertex", "shaders/framebuffers_screen.frag");

            // skybox reflection shader
            skyboxReflectShader = engine::Shader("cubemap", "shaders/cubemap.vertex", "shaders/cubemap.frag");

            simpleDepthShader = engine::Shader("simpleDepthBuffer", "shaders/shadow_mapping_depth.vertex", "shaders/shadow_mapping_depth.frag");
            debugDepthQuad = engine::Shader("debugDepthQuad", "shaders/debug_quad_depth.vertex", "shaders/debug_quad_depth.frag");

            // screen quad VAO
            // ---------------
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(engine::screenQuadVertices), &engine::screenQuadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


            // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
            // -------------------------------------------------------------------------------------------
            phongShader.use();
            phongShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
            phongShader.setFloat("material.shininess", 32.0f);


            // shader configuration
            // --------------------
            screenShader.use();
            screenShader.setInt("screenTexture", 0);



            // Depth map framebuffer configuration (shadow map)
            // -----------------------------------
            glGenFramebuffers(1, &depthMapFramebuffer);
            // create depth texture
            glGenTextures(1, &textureDepthMapBuffer);
            glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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




            // color framebuffer configuration
            // -------------------------
            //glGenFramebuffers(1, &colorFramebuffer);
            //glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
            //// create a color attachment texture
            //glGenTextures(1, &textureColorbuffer);
            //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
            //// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
            //glGenRenderbuffers(1, &rbo);
            //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
            //// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
            //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            //{
            //    std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            //}
            //glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // uncomment this call to draw in wireframe polygons.
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL


        }


        // must be overridden in derived class
        virtual void init() = 0;

        // must be overridden in derived class
        virtual void update(Shader& shader) = 0;

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

                // input
                //processInput(window);

                
                // render
                // ------
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // 1. render depth of scene to texture (from light's perspective)
                // --------------------------------------------------------------
                glm::mat4 lightProjection, lightView;
                glm::mat4 lightSpaceMatrix;
                float near_plane = 1.0f, far_plane = 7.5f;
                lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
                lightSpaceMatrix = lightProjection * lightView;
                // render scene from light's point of view
                simpleDepthShader.use();
                simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
                glClear(GL_DEPTH_BUFFER_BIT);
                //glActiveTexture(GL_TEXTURE0);
                //glBindTexture(GL_TEXTURE_2D, woodTexture);
                update(simpleDepthShader);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // reset viewport
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // 2. render scene as normal using the generated depth/shadow map  
                // --------------------------------------------------------------
                blinnPhongShader.use();
                //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
                //glm::mat4 view = camera.GetViewMatrix();
                //blinnPhongShader.setMat4("projection", projection);
                //blinnPhongShader.setMat4("view", view);
                // set light uniforms
                //blinnPhongShader.setVec3("viewPos", camera.Position);
                blinnPhongShader.setVec3("lightPos", lightPos);
                blinnPhongShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
                //glActiveTexture(GL_TEXTURE0);
                //glBindTexture(GL_TEXTURE_2D, woodTexture);
                
                

                update(blinnPhongShader);

                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
                blinnPhongShader.setInt("material.shadowMap", 3); // texture 3

                // render Depth map to quad for visual debugging
                // ---------------------------------------------
                debugDepthQuad.use();
                debugDepthQuad.setFloat("near_plane", near_plane);
                debugDepthQuad.setFloat("far_plane", far_plane);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);





                // 1. render depth of scene to texture (from light's perspective)
                // --------------------------------------------------------------
                //glm::mat4 lightProjection, lightView;
                //glm::mat4 lightSpaceMatrix;
                //float near_plane = 1.0f, far_plane = 7.5f;
                //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                //lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
                //lightSpaceMatrix = lightProjection * lightView;
                //// render scene from light's point of view
                //simpleDepthShader.use();
                //simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

                //glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                //glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
                //glClear(GL_DEPTH_BUFFER_BIT);
                //update(simpleDepthShader);
                //glBindFramebuffer(GL_FRAMEBUFFER, 0);

                //// reset viewport
                //glViewport(0, 0, width, height);
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                //// render Depth map to quad for visual debugging
                //// ---------------------------------------------
                //debugDepthQuad.use();
                //debugDepthQuad.setFloat("near_plane", near_plane);
                //debugDepthQuad.setFloat("far_plane", far_plane);
                //glActiveTexture(GL_TEXTURE0);
                //glBindTexture(GL_TEXTURE_2D, depthMapFramebuffer);
                //renderQuad();


















                // prepare the color framebuffer
                // bind to framebuffer and draw scene as we normally would to color texture 
                //glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
                //glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

                //// make sure we clear the framebuffer's content
                //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




                //// update function gets called here
                //update(blinnPhongShader);




                //// draw framebuffer to screen
                //// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
                //glBindFramebuffer(GL_FRAMEBUFFER, 0);
                //glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
                //// clear all relevant buffers
                //glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // set clear color (not really necessary actually, since we won't be able to see behind the quad anyways)
                //glClear(GL_COLOR_BUFFER_BIT);

                //screenShader.use();
                //glBindVertexArray(quadVAO);
                //glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
                //glDrawArrays(GL_TRIANGLES, 0, 6);






                // Rendering
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
            glDeleteVertexArrays(1, &quadVAO);
            glDeleteBuffers(1, &quadVBO);
            glDeleteRenderbuffers(1, &rbo);
            glDeleteFramebuffers(1, &colorFramebuffer);

            phongShader.clean();
            blinnPhongShader.clean();
            screenShader.clean();
            skyboxReflectShader.clean();

            clean();


            // imGui Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);

            glfwTerminate();
        }



    private:
        const int TARGET_FPS = 60;
        const int FRAME_DELAY = 1000 / TARGET_FPS; // in milliseconds


        bool key_w_pressed = false;

        bool show_window = false;



        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        unsigned int depthMapFramebuffer = 0;
        unsigned int colorFramebuffer = 0;


        unsigned int quadVAO = 0, quadVBO = 0;
        unsigned int rbo = 0;



        unsigned int textureDepthMapBuffer;
        unsigned int textureColorbuffer = 0;



        static void glfw_error_callback(int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
        }

        void renderUIWindow(bool show)
        {
            ImGui::SetNextWindowSize(ImVec2(480, 60), ImGuiCond_Always);

            ImGui::Begin("Hello, world!", &show);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            ImGui::End();
        }

        void renderQuad()
        {
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }


    protected:
        float framerate = 0.0f;

        // timing
        float deltaTime = 0.0f;	// time between current frame and last frame
        float lastFrame = 0.0f;

        // settings
        std::string title;
        unsigned int width = 0;
        unsigned int height = 0;
        const bool fullscreen = false;

    };
}


