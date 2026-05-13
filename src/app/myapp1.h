#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"

using namespace std;
using namespace engine;

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

    void init() override
    {

    }

    void update() override
    {

    }

    ~MyApp1() override
    {
        logger.trace("MyApp1 destructor called");
    }
};

