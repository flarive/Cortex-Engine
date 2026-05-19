#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp0.h" // app scenes switcher
#include "app/myapp1.h"



using namespace engine;

// make it easier to switch between scenes
using MyApp = MyApp0;


// Auto select Nvidia or AMD GPU instead of builtin intel GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


// Startup method
int main(int, char**)
{
    // Init the app
    AppManager appManager;
    auto app = appManager.createApp<MyApp>("MyApp", 1280, 720, false); //320, 240 //1280, 720
    if (!app)
        return EXIT_FAILURE;

    app->start();

    return EXIT_SUCCESS;
}
