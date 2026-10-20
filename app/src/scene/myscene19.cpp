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
        .iblDiffuseIntensity = 0.1f,
        .iblSpecularIntensity = 0.1f,
        .enableGammaCorrection = true,
        .enableBloom = true,
        .bloomThreshold = 1.0f,
        .bloomStrength = 0.03f,
        .bloomBlurPasses= 10
    })
{
    logger.trace("Scene {} constructor called", title);

    // my application specific state gets initialized here
}

MyScene19::MyScene19(const string& _title, std::weak_ptr<App> _app, const SceneSettings& _settings)
    : Scene(_title, _app, _settings)
{
    // my application specific state gets initialized here
}

void MyScene19::init()
{
    this->setUseBuiltinKeyboardNavigation(false);

    // cameras
    auto trsCamera1 = Transform{ { 0.0f, 0.0f, 5.0f } };
    auto camera1 = make_shared<FlyCamera>(48.0f, -90.0f, -2.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // lights
    auto trsLight1 = Transform{ {0.0f, 1.5f, 0.0f} };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(0.5f);
    light1->setAmbientColor(Color(0.1f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    // model
    const string modelPath = FileSystemManager::getPath("../../samples/models/EmissiveStrengthTest/glTF/EmissiveStrengthTest.glTF");
    shared_ptr<Model> sampleModel = make_shared<Model>(modelPath, false, false, false);
    auto trsSampleModel = Transform(vec3(0.0f, 0.0f, 0.0f), vec3(0.3f), vec3(0.0f, 0.0f, 0.0f));
    auto entitySampleModel = make_shared<Entity>("EmissiveStrengthTest");
    entitySampleModel->addComponent<TransformComponent>(trsSampleModel);
    entitySampleModel->addComponent<ModelComponent>(sampleModel);
    getEntityManager().addChild(entitySampleModel);
}

void MyScene19::onInputUpdate()
{
    Scene::onInputUpdate();

    auto camera = getActiveCamera();
    if (!camera)
        return;

    auto& input = getApp()->getInputManager();

    if (input.isKeyDown(GLFW_KEY_LEFT))
    {
        camera->processKeyboard(YAW_DOWN, deltaTime);
        camera->processKeyboard(LEFT, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_RIGHT))
    {
        camera->processKeyboard(YAW_UP, deltaTime);
        camera->processKeyboard(RIGHT, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_UP))
    {
        camera->processKeyboard(FORWARD, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_DOWN))
    {
        camera->processKeyboard(BACKWARD, deltaTime);
    }
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
