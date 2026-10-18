#include "../../include/scene/myscene0.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene0::MyScene0(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048
    })
{
    // my application specific state gets initialized here
}

void MyScene0::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 0.1f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(3.0f);
    light1->setCutoff(12.0f);
    light1->setOuterCutoff(48.0f);
    light1->setTarget(vec3(0.0f, 0.0f, 0.0f));
    light1->setAmbientColor(Color(1.0f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);
}

void MyScene0::onInputUpdate()
{
    Scene::onInputUpdate();

    auto camera = getActiveCamera();
    if (!camera)
        return;

    auto& input = getApp()->getInputManager();

    bool shiftPressed = input.isKeyDown(GLFW_KEY_LEFT_SHIFT) || input.isKeyDown(GLFW_KEY_RIGHT_SHIFT);

    if (shiftPressed)
    {
        if (input.isKeyDown(GLFW_KEY_LEFT))
            camera->processKeyboard(YAW_DOWN, deltaTime);

        if (input.isKeyDown(GLFW_KEY_RIGHT))
            camera->processKeyboard(YAW_UP, deltaTime);

        if (input.isKeyDown(GLFW_KEY_UP))
            camera->processKeyboard(PITCH_UP, deltaTime);

        if (input.isKeyDown(GLFW_KEY_DOWN))
            camera->processKeyboard(PITCH_DOWN, deltaTime);
    }
    else
    {
        if (input.isKeyDown(GLFW_KEY_LEFT))
            camera->processKeyboard(LEFT, deltaTime);

        if (input.isKeyDown(GLFW_KEY_RIGHT))
            camera->processKeyboard(RIGHT, deltaTime);

        if (input.isKeyDown(GLFW_KEY_UP))
            camera->processKeyboard(FORWARD, deltaTime);

        if (input.isKeyDown(GLFW_KEY_DOWN))
            camera->processKeyboard(BACKWARD, deltaTime);
    }

    // handle mouse
    camera->processMouseMovement(input.getMouseDeltaX(), input.getMouseDeltaY());

    // handle mouse scroll
    camera->processMouseScroll(input.getScrollY());
}

void MyScene0::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene0::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene0::updateUI()
{
}

void MyScene0::clean()
{
    // clean up any resources
}

MyScene0::~MyScene0()
{
    logger.trace("Scene {} destructor called", title);
}
