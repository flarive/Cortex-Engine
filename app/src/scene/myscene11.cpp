#include "../../include/scene/myscene11.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene11::MyScene11(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .backgroundGradientColors{true, Colors::hexToNormalizedRGB("#a0a0a0"), Colors::hexToNormalizedRGB("#cccccc"), 0.75f},
        .showDebugGrid = true,
        .shadowIntensity = 2.0f
    })
{
    // my application specific state gets initialized here
}


void MyScene11::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 1.0f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(20.0f, -90.0f, -7.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);
    


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(12.0f);
    light1->setCutoff(12.0f);
    light1->setOuterCutoff(48.0f);
    light1->setTarget(vec3(0.0f, 0.0f, 0.0f));
    light1->setAmbientColor(Color(0.2f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<BlinnPhongMaterial>(Color(0.1f),
        "textures/pbr/plastered-stone-wall/plastered_stone_wall_diff_1k.jpg",
        "textures/pbr/plastered-stone-wall/plastered_stone_wall_spec_1k.jpg",
        "textures/pbr/plastered-stone-wall/plastered_stone_wall_nor_gl_1k.jpg"), UvMapping(1.0f));
    auto trsPlane = Transform(vec3(0.0f, 0.0f, 0.0f), vec3(3.0f), vec3(0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);




    // animated vampire model
    auto vampireCustomMat = std::make_shared<BlinnPhongMaterial>(Color(0.1f), "models/vampire/Vampire_diffuse.png", "models/vampire/Vampire_specular.png", "models/vampire/Vampire_normal.png", "", "models/vampire/Vampire_emission.png", "");
    auto vampireModel = make_shared<Model>("models/vampire/dancing_vampire.dae", vampireCustomMat, true, false, true);
    auto vampireDancingAnimation = make_shared<BoneAnimation>("DanceAnim", "models/vampire/dancing_vampire.dae", vampireModel, 0.6f);
    auto vampireAnimations = std::vector<std::shared_ptr<BoneAnimation>>();
    vampireAnimations.push_back(vampireDancingAnimation);
    auto vampireAnimator = make_shared<BonesAnimator>(vampireAnimations);
    auto trsVampire = Transform(vec3(-0.8f, 0.0f, 0.0f), vec3(0.5f), vec3(0.0f));
    auto entityVampire = make_shared<Entity>("MyVampire");
    entityVampire->addComponent<TransformComponent>(trsVampire);
    entityVampire->addComponent<ModelComponent>(vampireModel);
    entityVampire->addComponent<AnimatorComponent>(vampireAnimator);
    getEntityManager().addChild(entityVampire);



    // mixamo twist dance model
    auto mixamoCustomMat = std::make_shared<BlinnPhongMaterial>(Color(0.1f), Color(0.9f, 0.1f, 0.1f, 1.0f), Color(0.5f), 3.0f);
    auto mixamoModel = make_shared<Model>("models/mixamo/Idle.glb", mixamoCustomMat, true, false, true);
    auto mixamoAnimation1 = make_shared<BoneAnimation>("TwistDance", "models/mixamo/TwistDance.glb", mixamoModel, 0.4f);
    auto mixamoAnimation2 = make_shared<BoneAnimation>("HipHopDance", "models/mixamo/HipHopDance.glb", mixamoModel, 0.4f);
    auto mixamoAnimations = std::vector<std::shared_ptr<BoneAnimation>>();
    mixamoAnimations.push_back(mixamoAnimation1);
    mixamoAnimations.push_back(mixamoAnimation2);
    auto mixamoAnimator = make_shared<BonesAnimator>(mixamoAnimations);
    auto trsMixamo = Transform(vec3(0.8f, 0.0f, 0.0f), vec3(0.5f), vec3(0.0f));
    auto entityMixamo = make_shared<Entity>("MyMixamo");
    entityMixamo->addComponent<TransformComponent>(trsMixamo);
    entityMixamo->addComponent<ModelComponent>(mixamoModel);
    entityMixamo->addComponent<AnimatorComponent>(mixamoAnimator);
    getEntityManager().addChild(entityMixamo);






    textFPSCount.setup(getApp()->window, FONT_PATH, 28);
    textPolyCount.setup(getApp()->window, FONT_PATH, 28);
    textMeshCount.setup(getApp()->window, FONT_PATH, 28);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 28);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 28);
    textTotalCount.setup(getApp()->window, FONT_PATH, 28);
}

void MyScene11::onInputUpdate()
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

void MyScene11::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene11::update(Shader& shader)
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

void MyScene11::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}

void MyScene11::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

MyScene11::~MyScene11()
{
    logger.trace("Scene {} destructor called", title);
}
