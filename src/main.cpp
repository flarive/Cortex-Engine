#include "core/include/app/app.h"
#include "core/include/app/scene.h"

#include "app/myapp1.h"

#include "app/myscene1.h"
#include "app/myscene2.h"
#include "app/myscene3.h"
#include "app/myscene4.h"
#include "app/myscene5.h"
#include "app/myscene6.h"
#include "app/myscene7.h"



// make it easier to switch between apps
using MyApp = MyApp1;
using MyScene = MyScene3;


engine::App* app{};
engine::Scene* scene{};





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
static void gamepadUpdate();



// Startup method
int main(int, char**)
{
    app = new MyApp("MyApp", 1280, 720, false);
    if (app)
    {
        scene = new MyScene("MyScene", app);
        if (scene)
        {
            scene->initialize();

            glfwSetFramebufferSizeCallback(scene->getWindow(), framebufferSizeCallback);
            glfwSetKeyCallback(scene->getWindow(), keyCallback);
            glfwSetCursorPosCallback(scene->getWindow(), mouseCallback);
            glfwSetScrollCallback(scene->getWindow(), scrollCallback);

            int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
            if (present > 0)
            {
                const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
                std::cout << "Joystick present " << name << std::endl;
            }

            // start game loop
            while (app->isRunning())
            {
                gamepadUpdate(); // Update gamepad state
                scene->gameLoop();
            }

            scene->exit();
            app->exit();
        }
    }
    else
    {
        std::cerr << "Failed to create application" << std::endl;
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNREFERENCED_PARAMETER(window);

    MyScene* myScene = (MyScene*)scene;
    myScene->key_callback(key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    UNREFERENCED_PARAMETER(window);

    MyScene* myScene = (MyScene*)scene;
    myScene->mouse_callback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    UNREFERENCED_PARAMETER(window);

    MyScene* myScene = (MyScene*)scene;
    myScene->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    UNREFERENCED_PARAMETER(window);

    MyScene* myScene = (MyScene*)scene;
    myScene->framebuffer_size_callback(width, height);
}

// Poll gamepad input and forward to MyApp
// ---------------------------------------
static void gamepadUpdate()
{
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            MyScene* myApp = (MyScene*)scene;
            myApp->gamepad_callback(state);
        }
    }
}
