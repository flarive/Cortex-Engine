#pragma once

#include "core/include/app/app.h"
#include "core/include/engine.h"


class MyApp1 : public engine::App
{

public:
    MyApp1(std::string _title, unsigned int _width = 800, unsigned int _height = 600, bool _fullscreen = false)
        : engine::App(_title, _width, _height, _fullscreen, engine::AppSettings
            {
                 .targetFPS = 60
            })
    {
        // my application specific state gets initialized here

    }
};

