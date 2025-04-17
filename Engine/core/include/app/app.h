#pragma once

#include "../engine.h"
#include "../tools/file_system.h"
#include "../tools/system_monitor.h"




#include <iostream>




namespace engine
{
    
    class Scene;
    
    struct AppSettings
    {
        unsigned int targetFPS{ 30 };
    };
    
    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App : private NonCopyable
    {
    private:

        SystemMonitor sysMonitor{};

        

    
    protected:
        float framerate{};

        // timing
        float deltaTime{}; // time between current frame and last frame
        float lastFrame{};



    public:
        GLFWwindow* window{};

        // settings
        std::string title{};
        int width{}; // windowed width
        int height{}; // windowed height
        bool fullscreen{};

        AppSettings settings;

        Scene* currentScene;
        
        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
            : title(_title), width(_width), height(_height), fullscreen(_fullscreen), settings(_settings)
        {
            setup();
        }

        const int getFrameDelay()
        {
            return 1000 / settings.targetFPS; // in milliseconds
        }

        void setCurrentScene(engine::Scene* scene)
        {
            currentScene = scene;
        }


        bool isRunning()
        {
            return !glfwWindowShouldClose(window);
        }


        void exit()
        {
            // imGui Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);

            glfwTerminate();
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
            //currentScene->refreshFullscreen();

            isFullscreen = !isFullscreen;
        }

        void renderUIWindow(bool show)
        {
            ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Always);

            ImGui::Begin("Hello, world!", &show);

            ImGui::Text("CPU Usage: %.2f%%", sysMonitor.GetCPUUsage());
            ImGui::Text("Memory Usage: %.2f Mo", sysMonitor.GetMemoryUsage());
            ImGui::Text("GPU Vendor: %s", sysMonitor.GetGPUVendor().c_str());
            ImGui::Text("GPU Renderer: %s", sysMonitor.GetGPURenderer().c_str());
            ImGui::Text("OpenGL Version: %s", sysMonitor.GetGPUVersion().c_str());

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
            ImGui::End();
        }


    private:

        void setup()
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
        }


        void initGLFW()
        {
            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit())
            {
                std::cerr << "GLFW init failed" << std::endl;
                std::exit(EXIT_FAILURE);
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
                std::exit(EXIT_FAILURE);
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
                std::exit(EXIT_FAILURE);
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

        static void glfw_error_callback(int error, const char* description)
        {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
            std::exit(EXIT_FAILURE);
        }
    };
}