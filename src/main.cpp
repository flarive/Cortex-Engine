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
//Scene* myScene{}; // non-owning observer

static std::weak_ptr<Scene> gScene;

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
    std::weak_ptr<App> myApp = appManager.createApp<MyApp>("MyApp", 1280, 720, false); //320, 240
    if (auto appShared = myApp.lock())
    {
        // Init a scene in the app
        appShared->getSceneManager().loadScene(std::make_shared<MyScene>("MyScene", appShared));

        // Observe only
        gScene = appShared->getSceneManager().getCurrentScene();
        
        if (auto scene = gScene.lock()) {
            scene->initialize();

            glfwSetFramebufferSizeCallback(scene->getWindow(), framebufferSizeCallback);
            glfwSetKeyCallback(scene->getWindow(), keyCallback);
            glfwSetCursorPosCallback(scene->getWindow(), mouseCallback);
            glfwSetScrollCallback(scene->getWindow(), scrollCallback);
            glfwSetWindowRefreshCallback(scene->getWindow(), windowRefreshCallback);
        }

            

        int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        if (present > 0)
        {
            const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
            logger.info("Joystick present {}", name);
        }

        // start game loop
        while (appShared->isRunning())
        {
            gamepadUpdate(); // Update gamepad state

            if (auto scene = gScene.lock()) {
                scene->gameLoop();
            }


            if (appShared->shouldUnloadScene())
            {
                appShared->getSceneManager().unloadCurrentScene();
                gScene.reset();

                // Clear the screen to black
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Swap buffers to ensure the clear is visible
                if (auto app = myApp.lock())
                {
                    glfwSwapBuffers(app->window);
                }
            }
        }

        if (auto scene = gScene.lock()) {
            scene->exit();
        }
        appShared->exit();
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;   //Do nothing

    if (auto scene = gScene.lock())
    {
        (static_cast<MyScene*>(scene.get()))->key_callback(key, scancode, action, mods);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;   //Do nothing

    if (auto scene = gScene.lock())
    {
        (static_cast<MyScene*>(scene.get()))->mouse_callback(xposIn, yposIn);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;   //Do nothing

    if (auto scene = gScene.lock())
    {
        (static_cast<MyScene*>(scene.get()))->scroll_callback(xoffset, yoffset);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;   //Do nothing

    if (auto scene = gScene.lock())
    {
        (static_cast<MyScene*>(scene.get()))->framebuffer_size_callback(width, height);
    }
}

static void windowRefreshCallback(GLFWwindow* window)
{
    (void)window;   //Do nothing

    if (auto scene = gScene.lock())
    {
        (static_cast<MyScene*>(scene.get()))->window_refresh_callback();
    }
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
            if (auto scene = gScene.lock())
            {
                (static_cast<MyScene*>(scene.get()))->gamepad_callback(state);
            }
        }
    }
}
