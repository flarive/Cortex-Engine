#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp1.h"

#include "app/myscene1.h" // blinnphong with skybox
#include "app/myscene2.h" // blinnphong cushion
#include "app/myscene3.h" // pbr balls with HDR background
#include "app/myscene4.h" // pbr cushion
#include "app/myscene5.h" // pbr buddha
#include "app/myscene6.h" // pbr rotating helmet
#include "app/myscene7.h" // pbr multiple helmets
#include "app/myscene8.h" // monochromatic point shadow
#include "app/myscene9.h" // orbit camera
#include "app/myscene10.h" // area lights demo
#include "app/myscene11.h" // blinnphong animated character
#include "app/myscene12.h" // PBR wood plane with misc materials
#include "app/myscene13.h" // Particles

// https://github.com/Co3us/OpenGLParticleSystem
// https://stackoverflow.com/questions/57454921/what-causes-glsl-recompilation-of-vertex-shader-based-on-state

using namespace engine;

// make it easier to switch between apps
using MyApp = MyApp1;
using MyScene = MyScene3;

App* myApp{};
Scene* myScene{};


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
    // Init the app
    myApp = new MyApp("MyApp", 1280, 720, false);
    if (myApp)
    {
        // Init a scene in the app
        myScene = new MyScene("MyScene", myApp);
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
            while (myApp->isRunning())
            {
                gamepadUpdate(); // Update gamepad state
                myScene->gameLoop();
            }

            myScene->exit();
            myApp->exit();
        }
        else
        {
            logger.error("Failed to create the scene");
            return -1;
        }
    }
    else
    {
        logger.error("Failed to create application");
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;   //Do nothing

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
