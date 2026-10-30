#pragma once

#include "../engine.h"
//#include "../common_defines.h" better ??????????????

#include "../managers/filesystem_manager.h"

#include "app_settings.h"

#include "../debug/opengl_debug.h"
#include "../managers/scene_manager.h"
#include "../managers/log_manager.h"

#include <iostream>
#include <functional>
#include <memory>

namespace engine
{
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

        // Toggle Fullscreen
        void toggleFullscreen(std::function<void()> func);


        std::string& getTitle() { return m_title; }

        // returns a unique instance of managers per app (no copy !)
        SceneManager& getSceneManager() { return m_sceneManager; }
        InputManager& getInputManager() { return m_inputManager; }
    
        virtual void onInputUpdate();

        // events
        virtual void onResize(int width, int height);
        virtual void onRefresh();


		// store app instance into window user pointer to be able to call non-static member functions in static callbacks
        static App* getAppFromWindow(GLFWwindow* window)
        {
            return static_cast<App*>(glfwGetWindowUserPointer(window));
        }

        
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->getInputManager().onKey(key, scancode, action, mods);
            }

            // forward event to imGui
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        }

        static void mouseCallback(GLFWwindow* window, double x, double y)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->getInputManager().onMouseMove(x, y);
            }

            // forward event to imGui
            ImGui_ImplGlfw_CursorPosCallback(window, x, y);
        }

        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            if (auto* app = getAppFromWindow(window))
            {
                app->getInputManager().onMouseButton(button, action, mods);
            }

            // forward event to imGui
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        }

        static void scrollCallback(GLFWwindow* window, double x, double y)
        {
            if (auto* app = getAppFromWindow(window)) {
                app->getInputManager().onScroll(x, y);
            }

            // forward event to imGui
            ImGui_ImplGlfw_ScrollCallback(window, x, y);
        }

        static void joystickCallback(int jid, int event)
        {
            (void)jid;   // Do nothing with jid
            
            if (event == GLFW_CONNECTED)
            {
                logger.info("Gamepad connected");
            }
            else if (event == GLFW_DISCONNECTED)
            {
                logger.info("Gamepad disconnected");
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
        InputManager m_inputManager{};


        std::string m_title{};
        std::string m_title_suffix{};
        std::string m_title_prefix{ "Cortex |" };

        void registerWindowInputCallbacks();
        void unregisterWindowInputCallbacks();

        void initWindow();
        void initGLAD();
        virtual void initImGUI(const char* backend_version) = 0;

        static void glfw_error_callback(int error, const char* description);

    private:
        virtual void setup() = 0;

        virtual void initGLFW() = 0;
        virtual const char* initBackend() = 0;
  

        virtual void enableVerticalSync(bool enable) = 0;
        virtual void enableMouseCapture(bool enable) = 0;
    };
}