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
    // Init the app
    AppManager appManager;
    CmdLineSettings settings = appManager.buildAppSettings(argc, argv);
    //CmdLineSettings settings{ 320, 240, false };
    auto app = appManager.createApp<MyApp>("MyApp", settings);
    if (!app)
        return EXIT_FAILURE;

    app->start();

    return EXIT_SUCCESS;
}
