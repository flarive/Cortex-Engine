#include "../../include/scene/myscene19.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene19::MyScene19(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .enableShadows = true,
        .shadowIntensity = 1.0f,
        .shadowMapsTextureSize = 2048,
        .shadowMapsBiasFactor = 0.015f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true
    })
{
    logger.trace("Scene {} constructor called", title);

    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

MyScene19::MyScene19(const string& _title, std::weak_ptr<App> _app, const SceneSettings& _settings)
    : Scene(_title, _app, _settings)
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene19::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, 0.0f, 5.0f } };
    auto camera1 = make_shared<FlyCamera>(78.0f, -90.0f, -2.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // lights
    auto trsLight1 = Transform{ {0.0f, 1.5f, 0.0f} };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(3.0f);
    light1->setAmbientColor(Color(0.1f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    const string modelPath = FileSystemManager::getPath("../../samples/models/EmissiveStrengthTest/glTF/EmissiveStrengthTest.glTF");
    shared_ptr<Model> sampleModel = make_shared<Model>(modelPath, false, false, false);
    auto trsSampleModel = Transform(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entitySampleModel = make_shared<Entity>("EmissiveStrengthTest");
    entitySampleModel->addComponent<TransformComponent>(trsSampleModel);
    entitySampleModel->addComponent<ModelComponent>(sampleModel);
    getEntityManager().addChild(entitySampleModel);
}


void MyScene19::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(LEFT, deltaTime);
        getActiveCamera()->processKeyboard(YAW_DOWN, deltaTime);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(RIGHT, deltaTime);
        getActiveCamera()->processKeyboard(YAW_UP, deltaTime);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(FORWARD, deltaTime);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(BACKWARD, deltaTime);
    }
}

void MyScene19::mouse_callback(double xposIn, double yposIn)
{
    Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode)
        return;

    float xpos{ static_cast<float>(xposIn) };
    float ypos{ static_cast<float>(yposIn) };

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset{ xpos - lastX };
    float yoffset{ lastY - ypos }; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    getActiveCamera()->processMouseMovement(xoffset, yoffset);
}

void MyScene19::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    if (is_editor_mode)
        return;

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene19::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene19::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene19::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene19::updateUI()
{
    // render HUD / UI
}

void MyScene19::clean()
{
}

MyScene19::~MyScene19()
{
    logger.trace("Scene {} destructor called", title);
}
