#include "../../include/scene/myscene17.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene17::MyScene17(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
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
        .iblDiffuseIntensity = 1.1f,
        .iblSpecularIntensity = 0.0f,
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
    // camera
    auto trsCamera1 = Transform{ { 0.0f, -12.0f, 2.0f } };
    auto camera1 = make_shared<FpsCamera>(45.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);



    // lights
    auto trsLight5 = Transform{ { 0.0f, 4.0f, -2.0f } };
    auto light5 = make_shared<SpotLight>();
    light5->setIntensity(10.0f);
    light5->setCutoff(12.5f);
    light5->setOuterCutoff(27.5f);
    light5->setTarget(vec3(0.0f, 0.0f, -4.0f));
    auto entityLight5 = make_shared<Entity>("Light5");
    entityLight5->addComponent<TransformComponent>(trsLight5);
    entityLight5->addComponent<LightComponent>(light5);
    getEntityManager().addChild(entityLight5);



    auto trsLight1 = Transform{ { -10.0f, 10.0f, 10.0f } };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(10.0f);
    light1->setDiffuseColor(Color(0.8f, 0.2f, 0.1f, 1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    auto trsLight2 = Transform{ { 10.0f, 10.0f, 10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(10.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{ { -10.0f, -10.0f, 10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(10.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{ { 10.0f, -10.0f, 10.0f } };
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(10.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);




    // ground
    auto myPlane = make_shared<Plane>();
    auto matPlane = make_shared<PBRMaterial>(Color(0.2f),
        "textures/pbr/painted-wood/PaintedWood_Color.jpg",
        "textures/pbr/painted-wood/PaintedWood_Normal.jpg",
        "textures/pbr/painted-wood/PaintedWood_Metallic.jpg",
        "textures/pbr/painted-wood/PaintedWood_Roughness.jpg",
        "textures/pbr/painted-wood/PaintedWood_AmbientOcclusion.jpg",
        "textures/pbr/painted-wood/PaintedWood_Displacement.jpg", "", "");
    matPlane->setNormalIntensity(1.0f);
    myPlane->setup(matPlane, UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -15.0f, -15.0f), vec3(12.0f), vec3(0.0f, 0.0f, 0.0f));
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
        "textures/pbr/ceramic/ClayCeramicGlossy_Displacement.jpg", "", "");
    matSphere1->setNormalIntensity(1.0f);

    sphere1->setup(matSphere1, UvMapping(1.0f));

    auto trsSphere1 = Transform(vec3(-3.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(sphere1);
    getEntityManager().addChild(entitySphere1);



    auto sphere2 = make_shared<Sphere>();

    auto matSphere2 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/porcelain/Porcelain_Color.png",
        "textures/pbr/porcelain/Porcelain_Normal.png",
        "textures/pbr/porcelain/Porcelain_Metallic.png",
        "textures/pbr/porcelain/Porcelain_Roughness.png",
        "textures/pbr/porcelain/Porcelain_AmbientOcclusion.png",
        "textures/pbr/porcelain/Porcelain_Displace.png", "", "");
    matSphere2->setNormalIntensity(1.0f);

    sphere2->setup(matSphere2, UvMapping(1.0f));

    auto trsSphere2 = Transform(vec3(0.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere2 = make_shared<Entity>("MySphere2");
    entitySphere2->addComponent<TransformComponent>(trsSphere2);
    entitySphere2->addComponent<PrimitiveComponent>(sphere2);
    getEntityManager().addChild(entitySphere2);


    auto sphere3 = make_shared<Sphere>();

    auto matSphere3 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/white-marble/white-marble_albedo.png",
        "textures/pbr/white-marble/white-marble_normal.png",
        "textures/pbr/white-marble/white-marble_metallic.png",
        "textures/pbr/white-marble/white-marble_roughness.png",
        "textures/pbr/white-marble/white-marble_ao.png",
        "textures/pbr/white-marble/white-marble_height.png", "", "");
    matSphere3->setNormalIntensity(1.0f);

    sphere3->setup(matSphere3, UvMapping(2.0f));

    auto trsSphere3 = Transform(vec3(3.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere3 = make_shared<Entity>("MySphere3");
    entitySphere3->addComponent<TransformComponent>(trsSphere3);
    entitySphere3->addComponent<PrimitiveComponent>(sphere3);
    getEntityManager().addChild(entitySphere3);
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
