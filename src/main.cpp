#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp1.h"

#include "app/myscene1.h" // blinnphong with skybox
#include "app/myscene2.h" // blinnphong cushion
#include "app/myscene3.h" // PBR balls with HDR background
#include "app/myscene4.h" // PBR cushion
#include "app/myscene5.h" // PBR buddha
#include "app/myscene6.h" // PBR rotating helmet
#include "app/myscene7.h" // PBR multiple helmets
#include "app/myscene8.h" // blinnphong monochromatic point shadow
#include "app/myscene9.h" // PBR orbit camera
#include "app/myscene10.h" // PBR area lights demo
#include "app/myscene11.h" // blinnphong animated character
#include "app/myscene12.h" // PBR wood plane with misc materials
#include "app/myscene13.h" // blinnphong particles
#include "app/myscene14.h" // blinnphong terrain
#include "app/myscene15.h" // parallax mapping

using namespace engine;

// make it easier to switch between scenes
using MyApp = MyApp1; 
using MyScene = MyScene15;

//App* myApp{}; // non-owning observer
Scene* myScene{}; // non-owning observer



// Auto select Nvidia or AMD GPU instead of builtin intel GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


// Needed by main method
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void windowRefreshCallback(GLFWwindow* window);
static void gamepadUpdate();


// Startup method
int main(int, char**)
{
    engine::AppManager appManager;

    // Init the app
    MyApp& myApp = appManager.createApp<MyApp>("MyApp", 320, 240, false);
    
    // Init a scene in the app
    myApp.getSceneManager().loadScene(std::make_shared<MyScene>("MyScene", &myApp));
    myScene = myApp.getSceneManager().getCurrentScene().get(); // convert smart to raw pointer (temp !)
    if (myScene)
    {
        myScene->initialize();

        glfwSetFramebufferSizeCallback(myScene->getWindow(), framebufferSizeCallback);
        glfwSetKeyCallback(myScene->getWindow(), keyCallback);
        glfwSetCursorPosCallback(myScene->getWindow(), mouseCallback);
        glfwSetScrollCallback(myScene->getWindow(), scrollCallback);
        glfwSetWindowRefreshCallback(myScene->getWindow(), windowRefreshCallback);

        int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        if (present > 0)
        {
            const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
            logger.info("Joystick present {}", name);
        }

        // start game loop
        while (myApp.isRunning())
        {
            gamepadUpdate(); // Update gamepad state
            if (myScene)
                myScene->gameLoop();
        }

        // Unregister all callbacks
        glfwSetFramebufferSizeCallback(myScene->getWindow(), nullptr);
        glfwSetKeyCallback(myScene->getWindow(), nullptr);
        glfwSetCursorPosCallback(myScene->getWindow(), nullptr);
        glfwSetScrollCallback(myScene->getWindow(), nullptr);
        glfwSetWindowRefreshCallback(myScene->getWindow(), nullptr);

        myScene->exit();
        myApp.exit();
    }
    else
    {
        logger.error("Failed to create the scene");
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;   //Do nothing
    if (myScene)
        ((MyScene*)myScene)->key_callback(key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->mouse_callback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->framebuffer_size_callback(width, height);
}

static void windowRefreshCallback(GLFWwindow* window)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->window_refresh_callback();
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
            ((MyScene*)myScene)->gamepad_callback(state);
        }
    }
}
