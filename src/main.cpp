#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp0.h" // app scenes switcher
#include "app/myapp1.h" // single scene app



using namespace engine;

// make it easier to switch between scenes
using MyApp = MyApp1;


// Auto select Nvidia or AMD GPU instead of builtin intel GPU
#define FORCE_DISCRETE_GPU

// Auto select Nvidia or AMD GPU instead of builtin intel GPU
#ifdef FORCE_DISCRETE_GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#else
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 0;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0;
}
#endif


// Startup method
int main(int, char**)
{
    // Init the app
    AppManager appManager;
    auto app = appManager.createApp<MyApp>("MyApp", 320, 240, false); //320, 240 //1280, 720
    if (!app)
        return EXIT_FAILURE;

    app->start();

    return EXIT_SUCCESS;
}
