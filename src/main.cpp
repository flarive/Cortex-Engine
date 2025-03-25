#include "core/include/app/app.h"
#include "app/myapp1.h"
#include "app/myapp2.h"
#include "app/myapp3.h"
#include "app/myapp4.h"


// make it easier to switch between apps
using MyApp = MyApp1;


engine::App* app{};


// Auto select Nvidia or AMD GPU instead of builtin intel GPU
#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif



static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Startup method
int main(int, char**)
{
    app = new MyApp("MyApp", 1280, 720, false);
    if (app)
    {
        glfwSetFramebufferSizeCallback(app->window, framebufferSizeCallback);
        glfwSetKeyCallback(app->window, keyCallback);
        glfwSetCursorPosCallback(app->window, mouseCallback);
        glfwSetScrollCallback(app->window, scrollCallback);

        // start game loop
        app->gameLoop();
    }
    else
    {
        std::cerr << "Failed to create application" << std::endl;
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNREFERENCED_PARAMETER(window);

    MyApp* myApp = (MyApp*)app;
    myApp->key_callback(key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    UNREFERENCED_PARAMETER(window);

    MyApp* myApp = (MyApp*)app;
    myApp->mouse_callback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    UNREFERENCED_PARAMETER(window);

    MyApp* myApp = (MyApp*)app;
    myApp->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    UNREFERENCED_PARAMETER(window);

    MyApp* myApp = (MyApp*)app;
    myApp->framebuffer_size_callback(width, height);
}








