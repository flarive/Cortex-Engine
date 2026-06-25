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

using MyScene = MyScene13;

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
        // Load scenes in the app
        m_sceneManager.addScene<MyScene>("MyScene", shared_from_this());



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

    void onKey(int key, int scancode, int action, int mods) override
    {
        App::onKey(key, scancode, action, mods);

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


    ~MyApp1() override
    {
        logger.trace("MyApp1 destructor called");
    }

private:
    weak_ptr<Scene> m_scene{};
};
