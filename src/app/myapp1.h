#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

#include "app/myscene1.h" // blinnphong with skybox
#include "app/myscene2.h" // blinnphong cushion
#include "app/myscene3.h" // PBR balls with HDR background
#include "app/myscene4.h" // PBR cushion
#include "app/myscene5.h" // PBR buddha
#include "app/myscene6.h" // PBR rotating helmet
#include "app/myscene7.h" // PBR multiple helmets
#include "app/myscene8.h" // blinnphong monochromatic point shadow
#include "app/myscene9.h" // PBR orbit camera
#include "app/myscene10.h" // PBR area lights demo
#include "app/myscene11.h" // blinnphong animated character
#include "app/myscene12.h" // PBR wood plane with misc materials
#include "app/myscene13.h" // blinnphong particles
#include "app/myscene14.h" // blinnphong terrain
#include "app/myscene15.h" // parallax mapping

using namespace std;
using namespace engine;

using MyScene = MyScene15;

class MyApp1 final : public App
{
public:
    MyApp1(const string& _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : App(_title, _width, _height, _fullscreen, AppSettings
            {
                 .targetFPS = 0
            })
    {
        logger.trace("MyApp1 constructor called");

        // my application specific state gets initialized here
    }

    void start() override
    {
        // Store the 'this' pointer in the window's user pointer
        glfwSetWindowUserPointer(window, this);

        // Set the callbacks
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetWindowRefreshCallback(window, windowRefreshCallback);

        // Load scenes in the app
        m_sceneManager.addScene(std::make_shared<MyScene>("Scene15", shared_from_this()));



        // Observe only
        m_scene = m_sceneManager.getCurrentScene();

        if (auto scene = m_scene.lock()) {
            scene->initialize();
        }



        //int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        //if (present > 0)
        //{
        //    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        //    logger.info("Joystick present {}", name);
        //}

        // start game loop
        while (isRunning())
        {
            //gamepadUpdate(); // Update gamepad state

            if (auto scene = m_scene.lock()) {
                scene->gameLoop();
            }
        }

        if (auto scene = m_scene.lock()) {
            scene->exit();
        }

        exit();
    }

    void key_callback(int key, int scancode, int action, int mods) override
    {
        App::key_callback(key, scancode, action, mods);
    }


    // glfw: whenever a keyboard key is pressed, this callback is called
    // -----------------------------------------------------------------
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // Retrieve the MyApp instance from the window's user pointer
        MyApp1* app = static_cast<MyApp1*>(glfwGetWindowUserPointer(window));
        if (app) {
            // Call the non-static key_callback method
            app->key_callback(key, scancode, action, mods);

            // Get the current scene and call its key_callback
            weak_ptr<Scene> gScene = app->getSceneManager().getCurrentScene();
            if (auto scene = gScene.lock()) {
                scene->key_callback(key, scancode, action, mods);
            }
        }
    }

    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
    {
        // Retrieve the MyApp instance from the window's user pointer
        MyApp1* app = static_cast<MyApp1*>(glfwGetWindowUserPointer(window));
        if (app) {
            // Get the current scene and call its key_callback
            weak_ptr<Scene> gScene = app->getSceneManager().getCurrentScene();
            if (auto scene = gScene.lock()) {
                scene->mouse_callback(xposIn, yposIn);
            }
        }
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    // ----------------------------------------------------------------------
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        (void)window;   //Do nothing

        //if (auto scene = gScene.lock())
        //{
        //    (static_cast<MyScene*>(scene.get()))->scroll_callback(xoffset, yoffset);
        //}
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        (void)window;   //Do nothing

        //if (auto scene = gScene.lock())
        //{
        //    (static_cast<MyScene*>(scene.get()))->framebuffer_size_callback(width, height);
        //}
    }

    static void windowRefreshCallback(GLFWwindow* window)
    {
        (void)window;   //Do nothing

        //if (auto scene = gScene.lock())
        //{
        //    (static_cast<MyScene*>(scene.get()))->window_refresh_callback();
        //}
    }

    ~MyApp1() override
    {
        logger.trace("MyApp1 destructor called");
    }

private:
    unsigned int m_currentSceneIndex{};
    weak_ptr<Scene> m_scene{};
};

