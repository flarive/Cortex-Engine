#include "../../include/scene/myscene17.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene17::MyScene17(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
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
    light1->setIntensity(4.0f);
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
        "textures/pbr/painted-wood/PaintedWood_Color.jpg",
        "textures/pbr/painted-wood/PaintedWood_Normal.jpg",
        "textures/pbr/painted-wood/PaintedWood_Metallic.jpg",
        "textures/pbr/painted-wood/PaintedWood_Roughness.jpg",
        "textures/pbr/painted-wood/PaintedWood_AmbientOcclusion.jpg",
        "textures/pbr/painted-wood/PaintedWood_Displacement.jpg", "", "");
    myPlane->setup(matPlane, UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(2.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);



    // sphere models
    auto sphere1 = make_shared<Sphere>();
    auto matSphere1 = make_shared<PBRMaterial>(Color(0.1f, 0.7f, 0.3f, 1.0f),
        "textures/pbr/ceramic/ClayCeramicGlossy_BaseColor.jpg",
        "textures/pbr/ceramic/ClayCeramicGlossy_Normal.jpg",
        "textures/pbr/ceramic/ClayCeramicGlossy_Metallic.jpg",
        "textures/pbr/ceramic/ClayCeramicGlossy_Roughness.jpg",
        "textures/pbr/ceramic/ClayCeramicGlossy_AmbientOcclusion.jpg",
        "textures/pbr/ceramic/ClayCeramicGlossy_Displacement.jpg");
    matSphere1->setNormalIntensity(1.0f);
    sphere1->setup(matSphere1, UvMapping(1.0f));
    auto trsSphere1 = Transform(vec3(0.0f, -0.35f, 0.0f), vec3(0.15f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(sphere1);
    //getEntityManager().addChild(entitySphere1);




    auto sphere3 = make_shared<Sphere>();
    auto matSphere3 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/white-marble/white-marble_albedo.png",
        "textures/pbr/white-marble/white-marble_normal.png",
        "textures/pbr/white-marble/white-marble_metallic.png",
        "textures/pbr/white-marble/white-marble_roughness.png",
        "textures/pbr/white-marble/white-marble_ao.png",
        "textures/pbr/white-marble/white-marble_height.png");
    matSphere3->setNormalIntensity(1.0f);
    sphere3->setup(matSphere3, UvMapping(2.0f));
    auto trsSphere3 = Transform(vec3(0.3f, -0.35f, 0.0f), vec3(0.15f));
    auto entitySphere3 = make_shared<Entity>("MySphere3");
    entitySphere3->addComponent<TransformComponent>(trsSphere3);
    entitySphere3->addComponent<PrimitiveComponent>(sphere3);
    //getEntityManager().addChild(entitySphere3);



    






    auto sphere2 = make_shared<Sphere>();
    auto matSphere2 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/porcelain/Porcelain_Color.png",
        "textures/pbr/porcelain/Porcelain_Normal.png",
        "textures/pbr/porcelain/Porcelain_Metallic.png",
        "textures/pbr/porcelain/Porcelain_Roughness.png",
        "textures/pbr/porcelain/Porcelain_AmbientOcclusion.png",
        "textures/pbr/porcelain/Porcelain_Displace.png");
    matSphere2->setNormalIntensity(1.0f);
    sphere2->setup(matSphere2, UvMapping(1.0f));
    auto trsSphere2 = Transform(vec3(-0.3f, -0.35f, 0.0f), vec3(0.15f));
    auto entitySphere2 = make_shared<Entity>("MySphere2");
    entitySphere2->addComponent<TransformComponent>(trsSphere2);
    entitySphere2->addComponent<PrimitiveComponent>(sphere2);
    //getEntityManager().addChild(entitySphere2);




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
    matSphere4->setNormalIntensity(1.0f);
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
    matSphere5->setNormalIntensity(1.0f);
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



    // helmet model
    //shared_ptr<Model> helmetModel = make_shared<Model>("models/helmet/DamagedHelmet.glTF", false, false, true);
    //auto trsHelmet = Transform(vec3(0.0f, 0.0f, 0.0f), vec3(2.0f));
    //auto entityHelmet = make_shared<Entity>("MyHelmet");
    //entityHelmet->addComponent<TransformComponent>(trsHelmet);
    //entityHelmet->addComponent<ModelComponent>(helmetModel);
    //getEntityManager().addChild(entityHelmet);
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

void MyScene17::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    if (is_editor_mode || show_demo_window)
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
