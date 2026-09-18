#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "include/app/myapp0.h" // app scenes switcher
#include "include/app/myapp1.h" // single scene app

#include <string>

using namespace engine;


// make it easier to switch between scenes
using MyApp = MyApp1;


// Startup method
// Command line args : -width 1280 -height 720 -fullscreen 0
int main(int argc, char* argv[])
{
    int width = 1280;
    int height = 720;
    bool fullscreen = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-width" && i + 1 < argc)
        {
            width = std::stoi(argv[++i]);
        }
        else if (arg == "-height" && i + 1 < argc)
        {
            height = std::stoi(argv[++i]);
        }
        else if (arg == "-fullscreen" && i + 1 < argc)
        {
            fullscreen = (std::stoi(argv[++i]) != 0);
        }
    }

    //width = 320;
    //height = 240;

    // Init the app
    AppManager appManager;
    auto app = appManager.createApp<MyApp>("MyApp", width, height, fullscreen); //320, 240 //1280, 720
    if (!app)
        return EXIT_FAILURE;

    app->start();

    return EXIT_SUCCESS;
}
