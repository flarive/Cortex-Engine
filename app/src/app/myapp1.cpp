#include "../../include/app/myapp1.h"

using namespace std;
using namespace glm;
using namespace engine;

MyApp1::MyApp1(const string& _title, unsigned int _width, unsigned int _height, bool _fullscreen)
    : App(_title, _width, _height, _fullscreen, AppSettings
        {
             .targetFPS = 0
        })
{
    logger.trace("MyApp1 constructor called");

    // my application specific state gets initialized here
}

void MyApp1::start()
{
    // Load scenes in the app
    m_sceneManager.addScene<MyScene>("MyScene", shared_from_this());



    // Observe only
    m_scene = m_sceneManager.getCurrentScene();

    if (auto scene = m_scene.lock()) {
        scene->initialize();
    }

    // Check if a gamepad is connected
    bool aa = getInputManager().isgamepadConnected();

    //int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    //if (present > 0)
    //{
    //    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    //    logger.info("Joystick present {}", name);
    //}

    // start game loop
    while (isRunning())
    {
        //gamepadUpdate(); // Update gamepad state

        if (auto scene = m_scene.lock()) {
            scene->gameLoop();
        }
    }

    if (auto scene = m_scene.lock()) {
        scene->exit();
    }

    exit();
}

void MyApp1::onInputUpdate()
{
    App::onInputUpdate();
}

void MyApp1::onResize(int w, int h)
{
    if (auto scene = m_scene.lock()) {
        scene->framebuffer_size_callback(w, h);
    }
}

void MyApp1::onRefresh()
{
    if (auto scene = m_scene.lock()) {
        scene->window_refresh_callback();
    }
}

MyApp1::~MyApp1()
{
    logger.trace("MyApp1 destructor called");
}
