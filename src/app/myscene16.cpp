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
        .shadowMapsBiasFactor = 0.050f
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
    // camera
    auto trsCamera1 = Transform{ {0.0f, 1.0f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);

    // light
    auto trsLight2 = Transform{ {0.0f, 2.0f, 3.0f} };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(8.0f);
    light2->setAmbientColor(Color(1.0f));
    light2->setDiffuseColor(Color(1.0f));
    light2->setSpecularColor(Color(1.0f));
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);


    // ground
    auto myPlane = make_shared<Plane>(false);
    shared_ptr<Material> matPlane{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matPlane = make_shared<PBRMaterial>(Color(0.1f),
            "textures/pbr/aerial-rocks/aerial_rocks_04_diff_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_nor_gl_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_metal_2k.png",
            "textures/pbr/aerial-rocks/aerial_rocks_04_rough_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_ao_2k.jpg",
            "textures/pbr/aerial-rocks/aerial_rocks_04_disp_2k.jpg");
    }
    else {
        matPlane = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");
    }
    matPlane->useParallaxMapping(true);
    myPlane->setup(matPlane, UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, -1.5f), vec3(3.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);





    // sphere
    auto mySphere1 = make_shared<Sphere>(false);
    shared_ptr<Material> matSphere1{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
            "textures/pbr/red-scifi-metal/albedo.png",
            "textures/pbr/red-scifi-metal/normal.png",
            "textures/pbr/red-scifi-metal/metallic.png",
            "textures/pbr/red-scifi-metal/roughness.png",
            "textures/pbr/red-scifi-metal/ao.png",
            "textures/pbr/red-scifi-metal/height.png");
    }
    else {
        matSphere1 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");
    }
    matSphere1->useParallaxMapping(true);
    mySphere1->setup(matSphere1, UvMapping(1.0f));
    auto trsSphere1 = Transform(vec3(0.0f, 0.36f, 0.0f), vec3(0.3f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    AnimTransform animSphere1{ trsSphere1, Transform(trsSphere1).addRotationY(360.0f), AnimMode::Absolute, 5.0f, true };
    auto trsSphereAnimation1 = make_shared<TransformAnimation>("animSphere1", animSphere1);
    auto trsSphereAnimator = make_shared<TransformAnimator>(trsSphereAnimation1);
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(mySphere1);
    //entitySphere1->addComponent<AnimatorComponent>(trsSphereAnimator);
    getEntityManager().addChild(entitySphere1);




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
