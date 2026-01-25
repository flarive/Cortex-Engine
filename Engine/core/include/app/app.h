#pragma once

#include "../engine.h"
#include "../tools/file_system.h"

#include "../debug/opengl_debug.h"

#include <iostream>
#include <functional>

namespace engine
{
    struct AppSettings
    {
        unsigned int targetFPS{ 0 }; // 0 means no FPS capping
    };

    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App : private NonCopyable
    {
    public:
        GLFWwindow* window{};

        // settings
        float width{}; // windowed width
        float height{}; // windowed height
        bool fullscreen{};

        AppSettings settings;

       
        App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
           

        const int getFrameDelay();
        const bool capFramerate();


        bool isRunning();



        void exit();



        void setWindowTitle(bool appendFps = true);

        void setWindowTitle(const std::string& title);

        void setWindowTitlePrefix(const std::string& prefix);
        void resetWindowTitlePrefix();

        void setWindowTitleSuffix(const std::string& suffix);
        void resetWindowTitleSuffix();



        // Toggle Fullscreen
        void toggleFullscreen(std::function<void()> func);


		std::string& getTitle() { return m_title; }

    private:
        std::string m_title{};
        std::string m_title_suffix{};
        std::string m_title_prefix{ "Cortex |" };

        void setup();

        void initGLFW();
        const char* initOpenGL();
        void initWindow();
        void initGLAD();
        void initImGUI(const char* glsl_version);
        
        static void GLAPIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

  

        void enableVerticalSync(bool enable);
        void enableMouseCapture(bool enable);

        static void glfw_error_callback(int error, const char* description);
    };
}