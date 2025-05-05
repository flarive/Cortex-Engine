#pragma once

#include "../engine.h"
#include "../tools/file_system.h"

#include <iostream>
#include <functional>

namespace engine
{
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

       
        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
           

        const int getFrameDelay();


        bool isRunning();



        void exit();



        // Toggle Fullscreen
        void toggleFullscreen(std::function<void()> func);
    

    private:

        void setup();


        void initGLFW();
        const char* initOpenGL();
        void initWindow();
        void initGLAD();
        void initImGUI(const char* glsl_version);
  

        void enableVerticalSync(bool enable);
        void enableMouseCapture(bool enable);

        static void glfw_error_callback(int error, const char* description);
    };
}