#include "../../include/scene/myscene2.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene2::MyScene2(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .shadowIntensity = 2.0f
    })
{
    // my application specific state gets initialized here
}


void MyScene2::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 0.0f, 5.0f} };
    auto camera1 = make_shared<FpsCamera>(25.0f, -90.0f, 0.0f, 1.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(7.0f);
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



    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<BlinnPhongMaterial>(Color(0.1f),
        "textures/wood_diffuse.png",
        "textures/wood_specular.png"), UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, -6.0f), vec3(10.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);




    // cushion model
    auto cushionModel = make_shared<Model>("models/cushion/cushion.obj");

    if (std::shared_ptr<engine::Material> cushionMaterial = cushionModel->getMaterial())
    {
        cushionMaterial->setNormalIntensity(2.0f);
        cushionMaterial->setSpecularIntensity(2.0f);
    }

    auto trsCushion = Transform(vec3(0.0f, 0.25f, 0.0f), vec3(0.5f), vec3(0.0f, 45.0f, 0.0f));
    auto entityCushion = make_shared<Entity>("MyCushion");
    entityCushion->addComponent<TransformComponent>(trsCushion);
    entityCushion->addComponent<ModelComponent>(cushionModel);
    getEntityManager().addChild(entityCushion);






    textFPSCount.setup(getApp()->window, FONT_PATH, 28);
    textPolyCount.setup(getApp()->window, FONT_PATH, 28);
    textMeshCount.setup(getApp()->window, FONT_PATH, 28);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 28);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 28);
    textTotalCount.setup(getApp()->window, FONT_PATH, 28);
}

void MyScene2::handleInput()
{
    Scene::handleInput();

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

void MyScene2::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene2::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene2::update(Shader& shader)
{
    (void)shader;   //Do nothing

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        auto& trs = myCushion->getTransform();
        trs.setLocalRotation(vec3(0.0f, rotation, 0.0f));
        myCushion->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene2::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}

void MyScene2::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

MyScene2::~MyScene2()
{
    logger.trace("Scene {} destructor called", title);
}

