#include "myscene15.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene15::MyScene15(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
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

MyScene15::MyScene15(const string& _title, std::weak_ptr<App> _app, const SceneSettings& _settings)
    : Scene(_title, _app, _settings)
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene15::init()
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
            "textures/bricks2.jpg",
            "textures/bricks2_normal.jpg",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/bricks2_disp.jpg");
    }
    else {
        matPlane = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");
        //auto matPlane = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");
    }
    matPlane->useParallaxMapping(true);
    myPlane->setup(matPlane, UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, -1.5f), vec3(3.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);

    


    auto myPlane2 = make_shared<Plane>(false);
    shared_ptr<Material> matPlane2{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matPlane2 = make_shared<PBRMaterial>(Color(0.1f),
            "textures/wood_diffuse.png",
            "textures/toy_box_normal.png",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/toy_box_disp.png");
    }
    else {
        matPlane2 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/wood_diffuse.png", "", "textures/toy_box_normal.png", "textures/toy_box_disp.png");
    }
    matPlane2->useParallaxMapping(true);
    myPlane2->setup(matPlane2, UvMapping(1.0f));
    auto trsPlane2 = Transform(vec3(0.0f, 2.0f, -1.5f), vec3(1.0f), vec3(-15.0f, 0.0, 180.0f));
    auto entityPlane2 = make_shared<Entity>("MyPlane2");
    entityPlane2->addComponent<TransformComponent>(trsPlane2);
    entityPlane2->addComponent<PrimitiveComponent>(myPlane2);
    getEntityManager().addChild(entityPlane2);



    auto myPlane3 = make_shared<Plane>(false);
    shared_ptr<Material> matPlane3{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matPlane3 = make_shared<PBRMaterial>(Color(0.1f),
            "textures/stones.png",
            "textures/stones_normal.png",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/stones_displace.png");
    }
    else {
        matPlane3 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/stones.png", "", "textures/stones_normal.png", "textures/stones_displace.png");
    }
    matPlane3->useParallaxMapping(true);
    myPlane3->setup(matPlane3, UvMapping(1.0f));
    auto trsPlane3 = Transform(vec3(-1.0f, 0.5f, -5.0f), vec3(1.0f), vec3(44.0f, 45.0, 0.0f));
    auto entityPlane3 = make_shared<Entity>("MyPlane3");
    entityPlane3->addComponent<TransformComponent>(trsPlane3);
    entityPlane3->addComponent<PrimitiveComponent>(myPlane3);
    getEntityManager().addChild(entityPlane3);



    auto myPlane4 = make_shared<Plane>(false);
    shared_ptr<Material> matPlane4{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matPlane4 = make_shared<PBRMaterial>(Color(0.1f),
            "textures/rocks.jpg",
            "textures/rocks_normal.png",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/rocks_displace.png");
    }
    else {
        matPlane4 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/rocks.jpg", "", "textures/rocks_normal.png", "textures/rocks_displace.png");
    }
    matPlane4->useParallaxMapping(true);
    myPlane4->setup(matPlane4, UvMapping(1.0f));
    auto trsPlane4 = Transform(vec3(1.0f, 0.5f, -5.0f), vec3(1.0f), vec3(44.0f, -45.0, 0.0f));
    auto entityPlane4 = make_shared<Entity>("MyPlane4");
    entityPlane4->addComponent<TransformComponent>(trsPlane4);
    entityPlane4->addComponent<PrimitiveComponent>(myPlane4);
    getEntityManager().addChild(entityPlane4);






    // sphere
    auto mySphere1 = make_shared<Sphere>(false);
    shared_ptr<Material> matSphere1{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
            "textures/bricks2.jpg",
            "textures/bricks2_normal.jpg",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/bricks2_disp.jpg");
        //auto matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
    //    "textures/pbr/planks/albedo.jpg",
    //    "textures/pbr/planks/normal.jpg",
    //    "textures/pbr/planks/metallic.jpg",
    //    "textures/pbr/planks/roughness.jpg",
    //    "textures/pbr/planks/ao.jpg",
    //    "textures/pbr/planks/displace.jpg");

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




    // cube
    auto myCube = make_shared<Cube>(2.0f);
    shared_ptr<Material> matCube{};
    if (this->getSceneSettings().method == RenderMethod::PBR) {
        matCube = make_shared<PBRMaterial>(Color(0.1f),
            "textures/bricks2.jpg",
            "textures/bricks2_normal.jpg",
            "textures/no_metalness.png",
            "textures/no_roughness.png",
            "textures/no_ao.png",
            "textures/bricks2_disp.jpg");
    }
    else {
        matCube = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/rocks.jpg", "", "textures/saint_normal.png", "textures/saint_displace.png");
    }
    matCube->useParallaxMapping(true);
    myCube->setup(matCube, UvMapping(1.0f));
    auto trsCube = Transform(vec3(0.0f, -0.35f, 0.0f), vec3(0.4f));
    auto entityCube = make_shared<Entity>("MyCube");
    AnimTransform anim1{ trsCube, Transform(trsCube).addRotationX(90.0f), AnimMode::Absolute, 2.0f, true };
    auto trsAnimation1 = make_shared<TransformAnimation>("anim1", anim1);
    auto trsAnimator = make_shared<TransformAnimator>(trsAnimation1);
    entityCube->addComponent<TransformComponent>(trsCube);
    entityCube->addComponent<PrimitiveComponent>(myCube);
    //entityCube->addComponent<AnimatorComponent>(trsAnimator);
    getEntityManager().addChild(entityCube);


    textFPSCount.setup(getApp()->window, FONT_PATH, 20);

    textPolyCount.setup(getApp()->window, FONT_PATH, 20);
    textMeshCount.setup(getApp()->window, FONT_PATH, 20);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 20);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 20);
    textTotalCount.setup(getApp()->window, FONT_PATH, 20);

    textIncrease.setup(getApp()->window, FONT_PATH, 18);
    textDecrease.setup(getApp()->window, FONT_PATH, 18);
    textParallaxIntensity.setup(getApp()->window, FONT_PATH, 18);

    textCurrentRenderer.setup(getApp()->window, FONT_PATH, 18);
}


void MyScene15::key_callback(int key, int scancode, int action, int mods)
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

    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementParallaxIntensity(-0.01f);
    else if (key == GLFW_KEY_KP_ADD && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementParallaxIntensity(0.01f);

    if (key == GLFW_KEY_KP_ENTER && action == GLFW_PRESS)
    {
        auto newMethod = this->getSceneSettings().method == RenderMethod::PBR ? RenderMethod::BlinnPhong : RenderMethod::PBR;
        switchRenderMode(newMethod);
    }
}

void MyScene15::mouse_callback(double xposIn, double yposIn)
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

void MyScene15::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene15::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene15::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene15::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene15::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);



    textIncrease.draw("NUMPAD + : increase parallax", getApp()->width - 200.0f, getApp()->height - 140.0f, 1.0f, Colors::White);
    textDecrease.draw("NUMPAD - : decrease parallax", getApp()->width - 200.0f, getApp()->height - 160.0f, 1.0f, Colors::White);
    textParallaxIntensity.draw(format("Parallax intensity : {}", m_parallaxIntensity), getApp()->width - 200.0f, getApp()->height - 180.0f, 1.0f, Colors::White);


    auto renderer = dynamic_cast<BlinnPhongRenderer*>(getRenderer());
    if (renderer)
    {
        textCurrentRenderer.draw(format("Renderer : {}", "BlinnPhong"), getApp()->width - 200.0f, getApp()->height - 220.0f, 1.0f, Colors::Orange);
    }
    else
    {
        textCurrentRenderer.draw(format("Renderer : {}", "PBR"), getApp()->width - 200.0f, getApp()->height - 220.0f, 1.0f, Colors::Green);
    }
}

void MyScene15::incrementParallaxIntensity(float intensity)
{
    auto clampParallaxIntensity = [](float value) {
        return std::clamp(value, 0.0f, 1.0f);
        };

    auto updateMaterial = [&](shared_ptr<Entity> entity) {
        if (entity) {
            if (auto component = entity->getComponent<PrimitiveComponent>()) {
                if (auto material = component->getPrimitive()->getMaterial()) {
                    float current = material->getParallaxIntensity();
                    m_parallaxIntensity = clampParallaxIntensity(current + intensity);
                    material->setParallaxIntensity(m_parallaxIntensity);
                }
            }
        }
    };

    updateMaterial(getEntityManager().findEntityByName("MySphere1"));
    updateMaterial(getEntityManager().findEntityByName("MyPlane"));
    updateMaterial(getEntityManager().findEntityByName("MyPlane2"));
    updateMaterial(getEntityManager().findEntityByName("MyPlane3"));
    updateMaterial(getEntityManager().findEntityByName("MyPlane4"));
    updateMaterial(getEntityManager().findEntityByName("MyCube"));
}

void MyScene15::switchRenderMode(RenderMethod method)
{
    (void)method;

    getApp()->getSceneManager().requestSceneUnload();
}

void MyScene15::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();

    textIncrease.clean();
    textDecrease.clean();
    textParallaxIntensity.clean();

    textCurrentRenderer.clean();
}

MyScene15::~MyScene15()
{
    logger.trace("Scene {} destructor called", title);
}
