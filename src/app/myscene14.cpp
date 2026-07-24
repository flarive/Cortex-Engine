#include "myscene14.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene14::MyScene14(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        //.method = RenderMethod::BlinnPhong,
        //.enableShadows = true,
        //.shadowIntensity = 3.0f,
        //.shadowMapsTextureSize = 2048

        .method = RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048,
        .iblDiffuseIntensity = 2.0f,
        .iblSpecularIntensity = 1.0f
    })
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}


void MyScene14::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 1.0f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(3.0f);
    light1->setCutoff(32.0f);
    light1->setOuterCutoff(48.0f);
    light1->setTarget(vec3(-1.0f, -2.0f, 1.0f));
    light1->setAmbientColor(Color(1.0f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    light1->setUseAttenuation(false);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


   /* auto trsLight1 = Transform{ { -10.0f, 10.0f, 10.0f } };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(1.0f);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    auto trsLight2 = Transform{ { 10.0f, 10.0f, 10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(1.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{ { -10.0f, -10.0f, 10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(1.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{ { 10.0f, -10.0f, 10.0f } };
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(1.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);

    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);*/


    // ground
    //auto myPlane = make_shared<Plane>();
    //myPlane->setup(make_shared<PBRMaterial>(Color(0.1f), "textures/uv_mapper_big.jpg"), UvMapping(1.0f));
    //auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(28.0f));
    //auto entityPlane = make_shared<Entity>("MyPlane");
    //entityPlane->addComponent<TransformComponent>(trsPlane);
    //entityPlane->addComponent<PrimitiveComponent>(myPlane);
    //getEntityManager().addChild(entityPlane);



    // terrain
    auto myTerrain = make_shared<Terrain>(3.2f, 10);
    shared_ptr<Material> matTerrain{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matTerrain = make_shared<PBRMaterial>(CombinedTexture::ARM, Color(0.1f),
            "textures/pbr/aerial-rocks/aerial_rocks_04_diff_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_nor_gl_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_arm_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_disp_2k.jpg");
    }
    else
    {
        matTerrain = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper_big.jpg", "", "", "textures/height/iceland_heightmap.png");
        //myTerrain->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/height/mountain_diffuse.jpg", "textures/height/mountain_specular.jpg", "textures/height/mountain_normal.jpg", "textures/height/mountain_height.jpg"), UvMapping(1.0f));
    }
    myTerrain->setup(matTerrain, UvMapping(1.0f));
    auto trsTerrain = Transform(vec3(0.0f, -5.0f, 0.0f), vec3(0.01f));
    auto entityTerrain = make_shared<Entity>("MyTerrain");
    entityTerrain->addComponent<TransformComponent>(trsTerrain);
    entityTerrain->addComponent<TerrainComponent>(myTerrain);
    getEntityManager().addChild(entityTerrain);



    // sphere
    auto mySphere1 = make_shared<Sphere>();
    shared_ptr<Material> matSphere1{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matSphere1 = make_shared<PBRMaterial>(CombinedTexture::ARM, Color(0.1f, 0.7f, 0.3f, 1.0f),
            "textures/pbr/ceramic/ClayCeramicGlossy_BaseColor.jpg",
            "textures/pbr/ceramic/ClayCeramicGlossy_Normal.jpg",
            "textures/pbr/ceramic/ClayCeramicGlossy_ARM.jpg",
            "textures/pbr/ceramic/ClayCeramicGlossy_Displacement.jpg");
    }
    else {
        matSphere1 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg");
    }
    mySphere1->setup(matSphere1, UvMapping(1.0f));
    auto trsSphere1 = Transform(vec3(0.0f, -4.0f, -3.0f), vec3(1.5f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(mySphere1);
    getEntityManager().addChild(entitySphere1);



    auto mySphere2 = make_shared<Sphere>();
    shared_ptr<Material> matSphere2{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matSphere2 = make_shared<PBRMaterial>(CombinedTexture::ARM, Color(0.1f),
            "textures/pbr/porcelain/Porcelain_Color.png",
            "textures/pbr/porcelain/Porcelain_Normal.png",
            "textures/pbr/porcelain/Porcelain_Arm.png",
            "textures/pbr/porcelain/Porcelain_Displace.png", 2.0f);
    }
    else {
        matSphere2 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg");
    }
    mySphere2->setup(matSphere2, UvMapping(1.0f));
    auto trsSphere2 = Transform(vec3(2.0f, 1.8f, -10.0f), vec3(1.2f));
    auto entitySphere2 = make_shared<Entity>("MySphere2");
    entitySphere2->addComponent<TransformComponent>(trsSphere2);
    entitySphere2->addComponent<PrimitiveComponent>(mySphere2);
    getEntityManager().addChild(entitySphere2);


    textFPSCount.setup(getApp()->window, FONT_PATH, 20);

    textPolyCount.setup(getApp()->window, FONT_PATH, 20);
    textMeshCount.setup(getApp()->window, FONT_PATH, 20);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 20);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 20);
    textTotalCount.setup(getApp()->window, FONT_PATH, 20);
}


void MyScene14::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(YAW_DOWN, deltaTime);
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(LEFT, deltaTime);

    if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(YAW_UP, deltaTime);
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(RIGHT, deltaTime);

    if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(PITCH_UP, deltaTime);
    else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(FORWARD, deltaTime);

    if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(PITCH_DOWN, deltaTime);
    else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(BACKWARD, deltaTime);
}

void MyScene14::mouse_callback(double xposIn, double yposIn)
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

void MyScene14::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene14::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene14::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene14::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene14::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}

void MyScene14::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

MyScene14::~MyScene14()
{
    logger.trace("Scene {} destructor called", title);
}
