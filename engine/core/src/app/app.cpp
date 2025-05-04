#include "../../include/app/app.h"


engine::App::App(std::string _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : title(_title), width(_width), height(_height), fullscreen(_fullscreen), settings(_settings)
{
    logger.info("Engine startup");

    setup();
}