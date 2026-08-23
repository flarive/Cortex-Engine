#include "myscene16.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene16::MyScene16(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        //.method = RenderMethod::BlinnPhong,
        //.enableShadows = true,
        //.shadowIntensity = 3.0f,
        //.shadowMapsTextureSize = 2048,
        //.shadowMapsBiasFactor = 0.050f

        .method = RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .enableShadows = true,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048,
        .shadowMapsBiasFactor = 0.050f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true, 
    })
{
    logger.trace("Scene {} constructor called", title);

    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

MyScene16::MyScene16(const string& _title, std::weak_ptr<App> _app, const SceneSettings& _settings)
    : Scene(_title, _app, _settings)
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene16::init()
{
    // cameras
    auto trsCamera1 = Transform{ {0.0f, -8.0f, 2.0f } };
    auto camera1 = make_shared<FlyCamera>(45.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);

    auto trsCamera2 = Transform{ { 0.0f, -9.0f, 2.0f } };
    auto camera2 = make_shared<FlyCamera>(20.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera2 = make_shared<Entity>("Camera2");
    entityCamera2->addComponent<TransformComponent>(trsCamera2);
    entityCamera2->addComponent<CameraComponent>(camera2);
    getEntityManager().addChild(entityCamera2);


    this->setActiveCamera(1);


    // lights
    auto trsLight1 = Transform{ { 0.0f, 8.0f, 0.0f } };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(20.0f);
    light1->setCutoff(10.0f);
    light1->setOuterCutoff(15.0f);
    light1->setTarget(vec3(0.0f, -11.0f, -10.0f));
    light1->setAmbientColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{ { -10.0f, 10.0f, 10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(10.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);


    // ground
    //auto myPlane = make_shared<Plane>(false);

    ////myPlane->setup(make_shared<PBRMaterial>(Color(0.2f),
    ////    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_diff_2k.ktx2",
    ////    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_nor_gl_2k.ktx2",
    ////    "textures/pbr/aerial-rocks/aerial_rocks_04_metal_2k.png",
    ////    "textures/pbr/aerial-rocks/aerial_rocks_04_rough_2k.jpg",
    ////    "textures/pbr/aerial-rocks/aerial_rocks_04_ao_2k.jpg",
    ////    ""), UvMapping(3.0f));

    //myPlane->setup(make_shared<PBRMaterial>(Color(0.1f),
    //    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_diff_2k.ktx2",
    //    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_nor_gl_2k.ktx2",
    //    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_metal_2k.ktx2",
    //    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_rough_2k.ktx2",
    //    "textures/pbr/aerial-rocks/ktx2/aerial_rocks_04_ao_2k.ktx2",
    //    ""), UvMapping(3.0f));


    //auto trsPlane = Transform(vec3(0.0f, -11.0f, -16.0f), vec3(12.0f), vec3(0.0f, 0.0f, 0.0f));
    //auto entityPlane = make_shared<Entity>("MyPlane");
    //entityPlane->addComponent<TransformComponent>(trsPlane);
    //entityPlane->addComponent<PrimitiveComponent>(myPlane);
    //getEntityManager().addChild(entityPlane);


    //// sphere models
    //auto sphere1 = make_shared<Sphere>();


    //auto matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
    //    "models/sphere/rounded-metal-cubes/albedo.png",
    //    "models/sphere/rounded-metal-cubes/normal.png",
    //    "models/sphere/rounded-metal-cubes/metallic.png",
    //    "models/sphere/rounded-metal-cubes/roughness.png",
    //    "models/sphere/rounded-metal-cubes/ao.png",
    //    "models/sphere/rounded-metal-cubes/height.png");



    //matSphere1->setNormalIntensity(5.0f);

    //sphere1->setup(matSphere1, UvMapping(2.0f));

    //auto trsSphere1 = Transform(vec3(-1.5f, -9.85f + 1.0f, -10.0f), vec3(1.0f), vec3(0.0f, 0.0f, 0.0f));
    //auto entitySphere1 = make_shared<Entity>("MySphere1");
    //entitySphere1->addComponent<TransformComponent>(trsSphere1);
    //entitySphere1->addComponent<PrimitiveComponent>(sphere1);
    //getEntityManager().addChild(entitySphere1);


    //// sphere models
    //auto sphere2 = make_shared<Sphere>();

    ////auto matSphere2 = make_shared<PBRMaterial>(Color(0.1f),
    ////    "textures/pbr/rusted_iron/albedo.png",
    ////    "textures/pbr/rusted_iron/normal.png",
    ////    "textures/pbr/rusted_iron/metallic.png",
    ////    "textures/pbr/rusted_iron/roughness.png",
    ////    "textures/pbr/rusted_iron/ao.png",
    ////    "textures/pbr/rusted_iron/height.png");

    ////auto matSphere2 = make_shared<PBRMaterial>(Color(0.1f),
    ////  "textures/pbr/rusted_iron/ktx2/albedo.ktx2",
    ////  "textures/pbr/rusted_iron/ktx2/normal.ktx2",
    ////  "textures/pbr/rusted_iron/ktx2/metallic.ktx2",
    ////  "textures/pbr/rusted_iron/ktx2/roughness.ktx2",
    ////  "textures/pbr/rusted_iron/ktx2/ao.ktx2",
    ////  "textures/pbr/rusted_iron/ktx2/height.ktx2");

    //auto matSphere2 = make_shared<PBRMaterial>(Color(0.1f),
    //    "models/sphere/rounded-metal-cubes/ktx2/albedo.ktx2",
    //    "models/sphere/rounded-metal-cubes/ktx2/normal.ktx2",
    //    "models/sphere/rounded-metal-cubes/ktx2/metallic.ktx2",
    //    "models/sphere/rounded-metal-cubes/ktx2/roughness.ktx2",
    //    "models/sphere/rounded-metal-cubes/ktx2/ao.ktx2",
    //    "models/sphere/rounded-metal-cubes/ktx2/height.ktx2");


    //matSphere2->setNormalIntensity(5.0f);

    //sphere2->setup(matSphere2, UvMapping(2.0f));

    //auto trsSphere2 = Transform(vec3(1.5f, -9.85f + 1.0f, -10.0f), vec3(1.0f), vec3(0.0f, 0.0f, 0.0f));
    //auto entitySphere2 = make_shared<Entity>("MySphere2");
    //entitySphere2->addComponent<TransformComponent>(trsSphere2);
    //entitySphere2->addComponent<PrimitiveComponent>(sphere2);
    //getEntityManager().addChild(entitySphere2);


    

    // Stained glass lamp model
    const string stainedGlassLampPath = FileSystemManager::getPath("../../samples/models/StainedGlassLamp/glTF/StainedGlassLamp.glTF");
    //const string stainedGlassLampPath = FileSystemManager::getPath("../../samples/models/StainedGlassLamp/glTF-KTX-BasisU/StainedGlassLamp.glTF");
    shared_ptr<Model> cushionModel = make_shared<Model>(stainedGlassLampPath);
    auto trsCushion = Transform(vec3(0.0f, -11.0f, -10.0f), vec3(6.0f));

    AnimTransform animLampRotate{ trsCushion, Transform(trsCushion).addRotationY(360.0f), AnimMode::Absolute, 30.0f, true };
    auto trsLampAnimation = make_shared<TransformAnimation>("animLampRotate", animLampRotate);
    auto trsLampAnimator = make_shared<TransformAnimator>(trsLampAnimation);

    auto entityLamp = make_shared<Entity>("StainedGlassLamp");
    entityLamp->addComponent<TransformComponent>(trsCushion);
    entityLamp->addComponent<ModelComponent>(cushionModel);
    entityLamp->addComponent<AnimatorComponent>(trsLampAnimator);
    getEntityManager().addChild(entityLamp);



    textFPSCount.setup(getApp()->window, FONT_PATH, 20);

    textPolyCount.setup(getApp()->window, FONT_PATH, 20);
    textMeshCount.setup(getApp()->window, FONT_PATH, 20);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 20);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 20);
    textTotalCount.setup(getApp()->window, FONT_PATH, 20);
}


void MyScene16::key_callback(int key, int scancode, int action, int mods)
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

void MyScene16::mouse_callback(double xposIn, double yposIn)
{
    Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode || show_demo_window)
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

void MyScene16::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    if (is_editor_mode || show_demo_window)
        return;

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene16::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene16::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene16::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene16::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}


void MyScene16::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

MyScene16::~MyScene16()
{
    logger.trace("Scene {} destructor called", title);
}
