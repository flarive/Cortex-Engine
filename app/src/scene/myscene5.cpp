#include "../../include/scene/myscene5.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene5::MyScene5(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .shadowIntensity = 0.9f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 0.3f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here
}

void MyScene5::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, -8.0f, 2.0f } };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);





    // lights
    auto trsLight1 = Transform{ { 0.0f, 6.0f, 0.0f } };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(50.0f);
    light1->setCutoff(12.5f);
    light1->setOuterCutoff(16.0f);
    light1->setTarget(vec3(0.0f, 0.0f, -5.0f));
    light1->setAmbientColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{ { -5.0f, -5.0f, -10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(40.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);

    auto trsLight3 = Transform{ { 5.0f, -5.0f, -10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(40.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);

    auto trsLight4 = Transform{ { 0.0f, -5.0f, -5.0f } };
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(40.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);


    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<PBRMaterial>(Color(0.2f),
        "textures/pbr/planks/albedo.jpg",
        "textures/pbr/planks/normal.jpg",
        "textures/pbr/planks/metallic.jpg",
        "textures/pbr/planks/roughness.jpg",
        "textures/pbr/planks/ao.jpg",
        ""), UvMapping(1.0f));

    auto trsPlane = Transform(vec3(0.0f, -11.0f, -16.0f), vec3(10.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // buddha model
    shared_ptr<Model> buddhaModel = make_shared<Model>("models/buddha/buddha1.obj");
    auto trsBuddha = Transform(vec3(0.0f, -11.0f + 1.0f, -10.0f), vec3(0.5f));
    auto entityBuddha = make_shared<Entity>("MyBuddha");
    entityBuddha->addComponent<TransformComponent>(trsBuddha);
    entityBuddha->addComponent<ModelComponent>(buddhaModel);
    getEntityManager().addChild(entityBuddha);

    ourText.setup(getApp()->window, FONT_PATH, 28);
}


void MyScene5::onInputUpdate()
{
    Scene::onInputUpdate();

    auto camera = getActiveCamera();
    if (!camera)
        return;

    auto& input = getApp()->getInputManager();

    bool shiftPressed = input.isKeyDown(GLFW_KEY_LEFT_SHIFT) || input.isKeyDown(GLFW_KEY_RIGHT_SHIFT);

    if (input.isKeyDown(GLFW_KEY_LEFT))
    {
        if (shiftPressed)
            camera->processKeyboard(YAW_DOWN, deltaTime);
        else
            camera->processKeyboard(LEFT, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_RIGHT))
    {
        if (shiftPressed)
            camera->processKeyboard(YAW_UP, deltaTime);
        else
            camera->processKeyboard(RIGHT, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_UP))
    {
        if (shiftPressed)
            camera->processKeyboard(PITCH_UP, deltaTime);
        else
            camera->processKeyboard(FORWARD, deltaTime);
    }

    if (input.isKeyDown(GLFW_KEY_DOWN))
    {
        if (shiftPressed)
            camera->processKeyboard(PITCH_DOWN, deltaTime);
        else
            camera->processKeyboard(BACKWARD, deltaTime);
    }

    // handle mouse
    camera->processMouseMovement(input.getMouseDeltaX(), input.getMouseDeltaY());

    // handle mouse scroll
    camera->processMouseScroll(input.getScrollY());
}

void MyScene5::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(getApp()->window, FONT_PATH, 28);
}

void MyScene5::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene5::updateUI()
{
    drawUI();
}

void MyScene5::clean()
{
    // clean up any resources
    ourText.clean();
}

void MyScene5::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing

    auto myBuddha = getEntityManager().findEntityByName("MyBuddha");
    if (myBuddha)
    {
        auto& trs = myBuddha->getTransform();
        trs.setLocalRotation(vec3(0.0f, rotation, 0.0f));
        myBuddha->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene5::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
}

MyScene5::~MyScene5()
{
    logger.trace("Scene {} destructor called", title);
}
