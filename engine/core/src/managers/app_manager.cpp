#include "../../include/managers/app_manager.h"

#include "../../include/app/app.h"

#include "../../include/managers/log_manager.h"


CmdLineSettings engine::AppManager::buildAppSettings(int argc, char* argv[])
{
    CmdLineSettings ret;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-width" && i + 1 < argc)
        {
            ret.width = std::stoi(argv[++i]);
        }
        else if (arg == "-height" && i + 1 < argc)
        {
            ret.height = std::stoi(argv[++i]);
        }
        else if (arg == "-fullscreen" && i + 1 < argc)
        {
            ret.fullscreen = (std::stoi(argv[++i]) != 0);
        }
    }

    return ret;
}

engine::AppManager::~AppManager()
{
	logger.trace("AppManager destructor called");
}
