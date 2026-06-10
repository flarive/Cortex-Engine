#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp0.h" // app scenes switcher
#include "app/myapp1.h" // single scene app



using namespace engine;

// make it easier to switch between scenes
using MyApp = MyApp1;


//Previous HEAD position was 4ec76fa Terrain shadows should work soon -> OK
//HEAD is now at 760fff8 Fix BlinnPhong terrain rendering -> KO

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
