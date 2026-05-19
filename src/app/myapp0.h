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

#include <memory>

using namespace std;
using namespace engine;

class MyApp0 final : public App
{
public:

    MyApp0(const string& _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : App(_title, _width, _height, _fullscreen, AppSettings
            {
                 .targetFPS = 0
            })
    {
        logger.trace("MyApp0 constructor called");

        // my application specific state gets initialized here
    }


    void start() override
    {
        // Set the callbacks
        registerCallbacks();

        // Set the callbacks
        /*glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetWindowRefreshCallback(window, windowRefreshCallback);*/

        //int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        //if (present > 0)
        //{
        //    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        //    logger.info("Joystick present {}", name);
        //}

        // Load scenes in the app
        m_sceneManager.addScene<MyScene1>("Scene1", shared_from_this());
        m_sceneManager.addScene<MyScene2>("Scene2", shared_from_this());
        m_sceneManager.addScene<MyScene3>("Scene3", shared_from_this());
        m_sceneManager.addScene<MyScene4>("Scene4", shared_from_this());
        m_sceneManager.addScene<MyScene5>("Scene5", shared_from_this());
        m_sceneManager.addScene<MyScene6>("Scene6", shared_from_this());
        m_sceneManager.addScene<MyScene7>("Scene7", shared_from_this());
        m_sceneManager.addScene<MyScene8>("Scene8", shared_from_this());
        m_sceneManager.addScene<MyScene9>("Scene9", shared_from_this());
        m_sceneManager.addScene<MyScene10>("Scene10", shared_from_this());
        m_sceneManager.addScene<MyScene11>("Scene11", shared_from_this());
        m_sceneManager.addScene<MyScene12>("Scene12", shared_from_this());
        m_sceneManager.addScene<MyScene13>("Scene13", shared_from_this());
        m_sceneManager.addScene<MyScene14>("Scene14", shared_from_this());
        m_sceneManager.addScene<MyScene15>("Scene15", shared_from_this());



        // Observe only
        m_scene = m_sceneManager.getCurrentScene();

        if (auto scene = m_scene.lock()) {
            scene->initialize();
        }


        // start game loop
        while (isRunning())
        {
            //gamepadCallback(window); // Update gamepad state

            if (auto scene = m_scene.lock()) {
                scene->gameLoop();
            }
        }

        if (auto scene = m_scene.lock()) {
            scene->exit();
        }

        exit();
    }

    void onKey(int key, int scancode, int action, int mods) override
    {
        App::onKey(key, scancode, action, mods);

        // Detect Shift key state
        bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        if (shiftPressed && key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            if (m_currentSceneIndex > 0)
            {
                m_currentSceneIndex--;

                // switch to previous scene
                auto previousScene = this->getSceneManager().setCurrentScene(m_currentSceneIndex);
                if (previousScene)
                {
                    logger.info("Switching to scene index {} ({})", m_currentSceneIndex, previousScene->getName());
                    // Observe only
                    m_scene = this->getSceneManager().getCurrentScene();
                    previousScene->initialize();
                    return;
                }
            }
        }

        if (shiftPressed && key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            if (m_currentSceneIndex < this->getSceneManager().getSceneCount())
            {
                m_currentSceneIndex++;

                // switch to next scene
                auto nextScene = this->getSceneManager().setCurrentScene(m_currentSceneIndex);
                if (nextScene)
                {
                    logger.info("Switching to scene index {} ({})", m_currentSceneIndex, nextScene->getName());
                    // Observe only
                    m_scene = this->getSceneManager().getCurrentScene();
                    nextScene->initialize();
                    return;
                }
            }
        }

        if (auto scene = m_scene.lock()) {
            scene->key_callback(key, scancode, action, mods);
        }
    }

    void onMouseMove(double x, double y) override
    {
        if (auto scene = m_scene.lock()) {
            scene->mouse_callback(x, y);
        }
    }

    void onScroll(double x, double y) override
    {
        if (auto scene = m_scene.lock()) {
            scene->scroll_callback(x, y);
        }
    }

    void onResize(int w, int h) override
    {
        if (auto scene = m_scene.lock()) {
            scene->framebuffer_size_callback(w, h);
        }
    }

    void onRefresh() override
    {
        if (auto scene = m_scene.lock()) {
            scene->window_refresh_callback();
        }
    }

    ~MyApp0() override
    {
        logger.trace("MyApp0 destructor called");
    }

    private:
        unsigned int m_currentSceneIndex{};
        weak_ptr<Scene> m_scene{};
};
