#include "include\app\app.h"
#include "myapp.h"



engine::App* app;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);


// Main code
int main(int, char**)
{
    app = new MyApp("MyApp", 640, 480, false);
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


static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(scancode);
    UNREFERENCED_PARAMETER(action);
    UNREFERENCED_PARAMETER(mods);

    MyApp* myApp = (MyApp*)app;
    myApp->keyCallback(window);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    MyApp* myApp = (MyApp*)app;
    myApp->mouse_callback(window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    MyApp* myApp = (MyApp*)app;
    myApp->scroll_callback(window, xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    MyApp* myApp = (MyApp*)app;
    myApp->framebuffer_size_callback(window, width, height);
}


