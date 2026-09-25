#include "../../include/app/myapp0.h"


using namespace std;
using namespace glm;
using namespace engine;


MyApp0::MyApp0(const string& _title, unsigned int _width, unsigned int _height, bool _fullscreen)
    : App(_title, _width, _height, _fullscreen, AppSettings
        {
             .targetFPS = 0
        })
{
    logger.trace("MyApp0 constructor called");

    // my application specific state gets initialized here
}


void MyApp0::start()
{
    // Set the callbacks
    /*glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowRefreshCallback(window, windowRefreshCallback);*/

    //int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    //if (present > 0)
    //{
    //    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    //    logger.info("Joystick present {}", name);
    //}

    // Load scenes in the app
    m_sceneManager.addScene<MyScene1>("Scene1", shared_from_this());
    m_sceneManager.addScene<MyScene2>("Scene2", shared_from_this());
    m_sceneManager.addScene<MyScene3>("Scene3", shared_from_this());
    m_sceneManager.addScene<MyScene4>("Scene4", shared_from_this());
    m_sceneManager.addScene<MyScene5>("Scene5", shared_from_this());
    m_sceneManager.addScene<MyScene6>("Scene6", shared_from_this());
    m_sceneManager.addScene<MyScene7>("Scene7", shared_from_this());
    m_sceneManager.addScene<MyScene8>("Scene8", shared_from_this());
    m_sceneManager.addScene<MyScene9>("Scene9", shared_from_this());
    m_sceneManager.addScene<MyScene10>("Scene10", shared_from_this());
    m_sceneManager.addScene<MyScene11>("Scene11", shared_from_this());
    m_sceneManager.addScene<MyScene12>("Scene12", shared_from_this());
    m_sceneManager.addScene<MyScene13>("Scene13", shared_from_this());
    m_sceneManager.addScene<MyScene14>("Scene14", shared_from_this());
    m_sceneManager.addScene<MyScene15>("Scene15", shared_from_this());
    m_sceneManager.addScene<MyScene16>("Scene16", shared_from_this());


    // Observe only
    m_scene = m_sceneManager.getCurrentScene();

    if (auto scene = m_scene.lock()) {
        scene->initialize();
    }


    // start game loop
    while (isRunning())
    {
        //gamepadCallback(window); // Update gamepad state

        if (auto scene = m_scene.lock()) {
            scene->gameLoop();
        }
    }

    if (auto scene = m_scene.lock()) {
        scene->exit();
    }

    exit();
}

void MyApp0::onKey(int key, int scancode, int action, int mods)
{
    App::onKey(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (shiftPressed && key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        if (m_currentSceneIndex > 0)
        {
            m_currentSceneIndex--;

            // switch to previous scene
            auto previousScene = this->getSceneManager().setCurrentScene(m_currentSceneIndex);
            if (previousScene)
            {
                logger.info("Switching to scene index {} ({})", m_currentSceneIndex, previousScene->getName());
                // Observe only
                m_scene = this->getSceneManager().getCurrentScene();
                previousScene->initialize();
                return;
            }
        }
    }

    if (shiftPressed && key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        if (m_currentSceneIndex < this->getSceneManager().getSceneCount())
        {
            m_currentSceneIndex++;

            // switch to next scene
            auto nextScene = this->getSceneManager().setCurrentScene(m_currentSceneIndex);
            if (nextScene)
            {
                logger.info("Switching to scene index {} ({})", m_currentSceneIndex, nextScene->getName());
                // Observe only
                m_scene = this->getSceneManager().getCurrentScene();
                nextScene->initialize();
                return;
            }
        }
    }

    if (auto scene = m_scene.lock()) {
        scene->key_callback(key, scancode, action, mods);
    }
}

void MyApp0::onMouseMove(double x, double y)
{
    if (auto scene = m_scene.lock()) {
        scene->mouse_callback(x, y);
    }
}

void MyApp0::onScroll(double x, double y)
{
    if (auto scene = m_scene.lock()) {
        scene->scroll_callback(x, y);
    }
}

void MyApp0::onResize(int w, int h)
{
    if (auto scene = m_scene.lock()) {
        scene->framebuffer_size_callback(w, h);
    }
}

void MyApp0::onRefresh()
{
    if (auto scene = m_scene.lock()) {
        scene->window_refresh_callback();
    }
}

MyApp0::~MyApp0()
{
    logger.trace("MyApp0 destructor called");
}
