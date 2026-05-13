#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

#include "myscene1.h" // blinnphong with skybox
#include "myscene2.h" // blinnphong cushion
#include "myscene3.h" // PBR balls with HDR background

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


    void init() override
    {
        // Load scenes in the app
        m_sceneManager.addScene(std::make_shared<MyScene1>("Scene1", shared_from_this()));
        m_sceneManager.addScene(std::make_shared<MyScene2>("Scene2", shared_from_this()));
        m_sceneManager.addScene(std::make_shared<MyScene3>("Scene3", shared_from_this()));

        // Observe only
        std::weak_ptr<Scene> gScene = m_sceneManager.getCurrentScene();

        if (auto scene = gScene.lock()) {
            scene->initialize();

            //glfwSetFramebufferSizeCallback(scene->getWindow(), framebufferSizeCallback);
            //glfwSetKeyCallback(scene->getWindow(), keyCallback);
            //glfwSetCursorPosCallback(scene->getWindow(), mouseCallback);
            //glfwSetScrollCallback(scene->getWindow(), scrollCallback);
            //glfwSetWindowRefreshCallback(scene->getWindow(), windowRefreshCallback);
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

            if (auto scene = gScene.lock()) {
                scene->gameLoop();
            }
        }

        if (auto scene = gScene.lock()) {
            scene->exit();
        }

        exit();
    }

    void update() override
    {

    }


    void key_callback(int key, int scancode, int action, int mods)
    {
        App::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        

        if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            // switch to previous scene
            //this->getSceneManager().requestSceneUnload();

            auto nextScene = this->getSceneManager().setCurrentScene(1);
            if (nextScene)
                nextScene->initialize();

            
        }

        if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            // switch to next scene
        }
    }

    ~MyApp0() override
    {
        logger.trace("MyApp0 destructor called");
    }
};
