#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

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

    void key_callback(int key, int scancode, int action, int mods)
    {
        App::key_callback(key, scancode, action, mods);

        // Detect Shift key state
        //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

        

        if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        {
            // switch to previous scene
            this->getSceneManager().requestSceneUnload();
            //this->getSceneManager().loadScene();
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

