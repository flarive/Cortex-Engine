#pragma once

#include "../engine.h"
#include "../tools/file_system.h"

#include "../debug/opengl_debug.h"
#include "../managers/scene_manager.h"

#include <iostream>
#include <functional>
#include <memory>

namespace engine
{
    struct AppSettings
    {
        unsigned int targetFPS{ 0 }; // 0 means no FPS capping
    };

    /// <summary>
    /// https://stackoverflow.com/questions/31581200/glfw-call-to-non-static-class-function-in-static-key-callback
    /// </summary>
    class App : private NonCopyable, public std::enable_shared_from_this<App> // to be able to create a smart pointer from this
    {
    public:
        GLFWwindow* window{};

        // settings
        float width{}; // windowed width
        float height{}; // windowed height
        bool fullscreen{};

        AppSettings settings;


        App(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
        virtual ~App();


        const int getFrameDelay();
        const bool capFramerate();


        bool isRunning();


        virtual void start() = 0;
        virtual void exit(); // not mandatory in derived class



        void setWindowTitle(bool appendFps = true);

        void setWindowTitle(const std::string& title);

        void setWindowTitlePrefix(const std::string& prefix);
        void resetWindowTitlePrefix();

        void setWindowTitleSuffix(const std::string& suffix);
        void resetWindowTitleSuffix();


        void setApp(GLFWwindow* _window);

        // retreive the app from GLFW window custom params (needed mostly GLFW callbacks static calls because C-style function pointers)
        //template<typename T>
        //static T* getApp(GLFWwindow* _window)
        //{
        //    return static_cast<T*>(glfwGetWindowUserPointer(_window));
        //}

        // Toggle Fullscreen
        void toggleFullscreen(std::function<void()> func);


        std::string& getTitle() { return m_title; }

        // returns a unique instance of managers per app (no copy !)
        SceneManager& getSceneManager() { return m_sceneManager; }
    

        virtual void onKey(int key, int scancode, int action, int mods);
        virtual void onMouseMove(double x, double y);
        virtual void onScroll(double xoffset, double yoffset);
        virtual void onResize(int width, int height);
        virtual void onRefresh();


        void registerCallbacks();


        static App* getAppFromWindow(GLFWwindow* window)
        {
            return static_cast<App*>(glfwGetWindowUserPointer(window));
        }

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->onKey(key, scancode, action, mods);
            }
        }

        static void mouseCallback(GLFWwindow* window, double x, double y)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->onMouseMove(x, y);
            }
        }

        static void scrollCallback(GLFWwindow* window, double x, double y)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->onScroll(x, y);
            }
        }

        static void framebufferSizeCallback(GLFWwindow* window, int w, int h)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->onResize(w, h);
            }
        }

        static void windowRefreshCallback(GLFWwindow* window)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->onRefresh();
            }
        }


    protected:
        SceneManager m_sceneManager{};

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