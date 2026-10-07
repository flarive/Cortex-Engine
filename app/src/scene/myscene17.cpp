#include "../../include/scene/myscene17.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene17::MyScene17(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
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

MyScene17::MyScene17(const string& _title, std::weak_ptr<App> _app, const SceneSettings& _settings)
    : Scene(_title, _app, _settings)
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene17::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, -0.2f, 3.0f } };
    auto camera1 = make_shared<FlyCamera>(18.0f, -90.0f, -2.0f, 10.0f);
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




    // ground
    auto myPlane = make_shared<Plane>(false);
    auto matPlane = make_shared<PBRMaterial>(Color(0.2f),
        "textures/pbr/old-wood-cracked-knots/old-wood-cracked-knots_albedo.jpg",
        "textures/pbr/old-wood-cracked-knots/old-wood-cracked-knots_normal.jpg",
        "",
        "textures/pbr/old-wood-cracked-knots/old-wood-cracked-knots_roughness.jpg",
        "textures/pbr/old-wood-cracked-knots/old-wood-cracked-knots_ao.jpg",
        "textures/pbr/old-wood-cracked-knots/old-wood-cracked-knots_height.jpg", "", "");
    myPlane->setup(matPlane, UvMapping(1.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(2.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // sphere models
    auto sphere4 = make_shared<Sphere>();
    auto matSphere4 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/frosted-glass/Glass_Frosted_001_basecolor.jpg",
        "textures/pbr/frosted-glass/Glass_Frosted_001_normal.jpg",
        "",
        "textures/pbr/frosted-glass/Glass_Frosted_001_roughness.jpg",
        "textures/pbr/frosted-glass/Glass_Frosted_001_ambientOcclusion.jpg",
        "textures/pbr/frosted-glass/Glass_Frosted_001_height.jpg",
        "",
        "textures/pbr/alpha_smooth.png");
    sphere4->setup(matSphere4, UvMapping(1.0f));
    auto trsSphere4 = Transform(vec3(-0.15f, -0.35f, 0.8f), vec3(0.15f));
    AnimTransform animSphere4{ trsSphere4, Transform(trsSphere4).addRotationY(360.0f), AnimMode::Absolute, 10.0f, true };
    auto trsSphereAnimation4 = make_shared<TransformAnimation>("animSphere4", animSphere4);
    auto trsSphereAnimator4 = make_shared<TransformAnimator>(trsSphereAnimation4);
    auto entitySphere4 = make_shared<Entity>("MySphere4");
    entitySphere4->addComponent<TransformComponent>(trsSphere4);
    entitySphere4->addComponent<PrimitiveComponent>(sphere4);
    entitySphere4->addComponent<AnimatorComponent>(trsSphereAnimator4);
    getEntityManager().addChild(entitySphere4);



    auto sphere5 = make_shared<Sphere>();
    auto matSphere5 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/glass-window/Glass_Window_004_basecolor.jpg",
        "textures/pbr/glass-window/Glass_Window_004_normal.jpg",
        "textures/pbr/glass-window/Glass_Window_004_metallic.jpg",
        "textures/pbr/glass-window/Glass_Window_004_roughness.jpg",
        "textures/pbr/glass-window/Glass_Window_004_ambientOcclusion.jpg",
        "textures/pbr/glass-window/Glass_Window_004_height.png",
        "",
        "textures/pbr/glass-window/Glass_Window_004_opacity.jpg");
    sphere5->setup(matSphere5, UvMapping(1.0f));
    auto trsSphere5 = Transform(vec3(0.15f, -0.35f, 0.8f), vec3(0.15f));
    AnimTransform animSphere5{ trsSphere5, Transform(trsSphere5).addRotationY(360.0f), AnimMode::Absolute, 15.0f, true };
    auto trsSphereAnimation5 = make_shared<TransformAnimation>("animSphere5", animSphere5);
    auto trsSphereAnimator5 = make_shared<TransformAnimator>(trsSphereAnimation5);
    auto entitySphere5 = make_shared<Entity>("MySphere5");
    entitySphere5->addComponent<TransformComponent>(trsSphere5);
    entitySphere5->addComponent<PrimitiveComponent>(sphere5);
    entitySphere5->addComponent<AnimatorComponent>(trsSphereAnimator5);
    getEntityManager().addChild(entitySphere5);



    auto sphere6 = make_shared<Sphere>();
    auto matSphere6 = make_shared<PBRMaterial>(Color(1.0f),
        "textures/pbr/pure-glass/albedo.png",
        "textures/pbr/pure-glass/normal.png",
        "textures/pbr/pure-glass/metallic.png",
        "textures/pbr/pure-glass/roughness.png",
        "textures/pbr/pure-glass/ao.png",
        "",
        "",
        "textures/pbr/pure-glass/opacity.png");
    matSphere6->setIOR(1.52f);
    matSphere6->setTransmission(1.0f);
    matSphere6->setThickness(0.5f);
    matSphere6->setAttenuationColor(Color(0.98f, 1.00f, 0.98f, 1.0f));
    matSphere6->setAttenuationDistance(5.0f);

    sphere6->setup(matSphere6, UvMapping(1.0f));
    auto trsSphere6 = Transform(vec3(0.0f, -0.35f, 1.6f), vec3(0.15f));


    AnimTransform anim1{ trsSphere6, Transform(trsSphere6).addTranslationY(0.4f), AnimMode::Absolute, 3.0f };
    auto trsAnimation1 = make_shared<TransformAnimation>("anim1", anim1);

    AnimTransform anim2{ trsSphere6, Transform(trsSphere6).addTranslationY(-0.4f) , AnimMode::Absolute , 3.0f };
    auto trsAnimation2 = make_shared<TransformAnimation>("anim2", anim2);


    auto transformAnimations = std::vector<std::shared_ptr<TransformAnimation>>();
    transformAnimations.push_back(trsAnimation1);
    transformAnimations.push_back(trsAnimation2);

    auto trsAnimator = make_shared<TransformAnimator>(transformAnimations);

    auto entitySphere6 = make_shared<Entity>("MySphere6");
    entitySphere6->addComponent<TransformComponent>(trsSphere6);
    entitySphere6->addComponent<PrimitiveComponent>(sphere6);
    entitySphere6->addComponent<AnimatorComponent>(trsAnimator);
    getEntityManager().addChild(entitySphere6);



    // helmet model
    auto helmetCustomMat = make_shared<PBRMaterial>(Color(1.0f),
        "textures/pbr/pure-glass/albedo.png",
        "textures/pbr/pure-glass/normal.png",
        "textures/pbr/pure-glass/metallic.png",
        "textures/pbr/pure-glass/roughness.png",
        "textures/pbr/pure-glass/ao.png",
        "",
        "",
        "textures/pbr/pure-glass/opacity.png");
    matSphere6->setIOR(1.52f);
    matSphere6->setTransmission(1.0f);
    matSphere6->setThickness(0.5f);
    matSphere6->setAttenuationColor(Color(0.98f, 1.00f, 0.98f, 1.0f));
    matSphere6->setAttenuationDistance(5.0f);

    shared_ptr<Model> helmetModel = make_shared<Model>("models/helmet/DamagedHelmet.glTF", helmetCustomMat, false, false, true);
    auto trsHelmet = Transform(vec3(0.65f, -0.31f, 0.0f), vec3(0.15f), vec3(0.0f, 180.0f, 0.0f));

    AnimTransform animHelmet{ trsHelmet, Transform(trsHelmet).addRotationY(360.0f), AnimMode::Absolute, 15.0f, true };
    auto trsHelmetAnimation = make_shared<TransformAnimation>("animHelmet", animHelmet);
    auto trsHelmetAnimator = make_shared<TransformAnimator>(trsHelmetAnimation);

    auto entityHelmet = make_shared<Entity>("MyHelmet");
    entityHelmet->addComponent<TransformComponent>(trsHelmet);
    entityHelmet->addComponent<ModelComponent>(helmetModel);
    entityHelmet->addComponent<AnimatorComponent>(trsHelmetAnimator);
    getEntityManager().addChild(entityHelmet);
}


void MyScene17::key_callback(int key, int scancode, int action, int mods)
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

void MyScene17::mouse_callback(double xposIn, double yposIn)
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

void MyScene17::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    if (is_editor_mode)
        return;

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene17::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene17::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene17::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene17::updateUI()
{
    // render HUD / UI
}

void MyScene17::clean()
{
}

MyScene17::~MyScene17()
{
    logger.trace("Scene {} destructor called", title);
}
