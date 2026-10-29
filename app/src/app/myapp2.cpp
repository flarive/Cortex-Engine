#include "../../include/app/myapp2.h"

using namespace std;
using namespace glm;
using namespace engine;

MyApp2::MyApp2(const string& _title, unsigned int _width, unsigned int _height, bool _fullscreen)
    : VulkanApp(_title, _width, _height, _fullscreen, AppSettings
        {
             .targetFPS = 0
        })
{
    logger.trace("MyApp2 constructor called");

    // my application specific state gets initialized here
}

void MyApp2::start()
{
    // Load scenes in the app
    m_sceneManager.addScene<MyApp2Scene>("MyScene", shared_from_this());



    // Observe only
    m_scene = m_sceneManager.getCurrentScene();

    if (auto scene = m_scene.lock()) {
        scene->initialize();
    }

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

void MyApp2::onInputUpdate()
{
    App::onInputUpdate();
}

void MyApp2::onResize(int w, int h)
{
    if (auto scene = m_scene.lock()) {
        scene->framebuffer_size_callback(w, h);
    }
}

void MyApp2::onRefresh()
{
    if (auto scene = m_scene.lock()) {
        scene->window_refresh_callback();
    }
}

MyApp2::~MyApp2()
{
    logger.trace("MyApp2 destructor called");
}
