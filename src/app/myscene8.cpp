#include "myscene8.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene8::MyScene8(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .shadowIntensity = 5.0f

        //.method = RenderMethod::PBR,
        //.HDRSkyboxHide = true,
        //.HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        //.HDRSkyboxBlurStrength = 0.0f,
        //.enableShadows = true,
        //.shadowIntensity = 3.0f,
        //.shadowMapsTextureSize = 2048,
        //.shadowMapsBiasFactor = 0.050f
    })
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene8::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, 0.0f, 3.0f } };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);






    // light
    auto trsLight1 = Transform{ {0.0f, 1.5f, 0.0f} };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(3.0f);
    light1->setAmbientColor(Color(1.0f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    //auto zzz = Colors::hexToNormalizedRGB("#FFF1AD");
    //auto zzz2 = Color(0.5f);
    //auto zzz3 = Color(0.5f);

    auto zzz = Colors::hexToNormalizedRGB("#FFF1AD");
    auto zzz2 = Colors::hexToNormalizedRGB("#FFFF99");
    auto zzz3 = Colors::hexToNormalizedRGB("#00FFFF");


    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f), UvMapping(6.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(2.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);



    // cube 1
    auto myCube1 = make_shared<Cube>();
    myCube1->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube1 = Transform(vec3(-1.0f, -0.35f, -1.0f), vec3(0.15f), vec3(0.0f, 0.0f, 0.0f));

    AnimTransform anim1{ trsCube1, Transform(trsCube1).addRotationX(90.0f).addRotationY(90.0f), AnimMode::Absolute, 10.0f };
    auto trsAnimation1 = make_shared<TransformAnimation>("anim1", anim1);

    AnimTransform anim2{ trsCube1, Transform(trsCube1).addTranslationX(2.0f) , AnimMode::Absolute , 5.0f };
    auto trsAnimation2 = make_shared<TransformAnimation>("anim2", anim2);


    auto transformAnimations = std::vector<std::shared_ptr<TransformAnimation>>();
    transformAnimations.push_back(trsAnimation1);
    transformAnimations.push_back(trsAnimation2);

    auto trsAnimator = make_shared<TransformAnimator>(transformAnimations);

    auto entityCube1 = make_shared<Entity>("MyCube1");
    entityCube1->addComponent<TransformComponent>(trsCube1);
    entityCube1->addComponent<PrimitiveComponent>(myCube1);
    entityCube1->addComponent<AnimatorComponent>(trsAnimator);
    getEntityManager().addChild(entityCube1);


    // cube 2
    auto myCube2 = make_shared<Cube>();
    myCube2->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube2 = Transform(vec3(1.0f, -0.35f, 1.0f), vec3(0.15f), vec3(0.0f, 0.0f, 0.0f));
    auto entityCube2 = make_shared<Entity>("MyCube2");
    entityCube2->addComponent<TransformComponent>(trsCube2);
    entityCube2->addComponent<PrimitiveComponent>(myCube2);
    getEntityManager().addChild(entityCube2);


    // cube 3
    auto myCube3 = make_shared<Cube>();
    myCube3->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube3 = Transform(vec3(1.0f, -0.35f, -1.0f), vec3(0.15f), vec3(0.0f, 0.0f, 0.0f));
    auto entityCube3 = make_shared<Entity>("MyCube3");
    entityCube3->addComponent<TransformComponent>(trsCube3);
    entityCube3->addComponent<PrimitiveComponent>(myCube3);
    getEntityManager().addChild(entityCube3);


    // cube 4
    auto myCube4 = make_shared<Cube>();
    myCube4->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube4 = Transform(vec3(-1.0f, -0.35f, 1.0f), vec3(0.15f), vec3(0.0f, 0.0f, 0.0f));
    auto entityCube4 = make_shared<Entity>("MyCube4");
    entityCube4->addComponent<TransformComponent>(trsCube4);
    entityCube4->addComponent<PrimitiveComponent>(myCube4);
    getEntityManager().addChild(entityCube4);


    // sphere
    auto mySphere = make_shared<Sphere>();
    mySphere->setup(make_shared<BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

    auto trsSphere = Transform(vec3(0.0f, -0.35f, 0.0f), vec3(0.15f), vec3(0.0f, 0.0f, 0.0f));
    auto entitySphere = make_shared<Entity>("MySphere");
    entitySphere->addComponent<TransformComponent>(trsSphere);
    entitySphere->addComponent<PrimitiveComponent>(mySphere);
    getEntityManager().addChild(entitySphere);



    ourText.setup(getApp()->window, FONT_PATH, 20);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene8::key_callback(int key, int scancode, int action, int mods)
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


void MyScene8::mouse_callback(double xposIn, double yposIn)
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

void MyScene8::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene8::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene8::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(getApp()->window, FONT_PATH, 20);
}

void MyScene8::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene8::updateUI()
{
    drawUI();
}

void MyScene8::clean()
{
    // clean up any resources
    ourSkybox.clean();
    ourText.clean();
}

void MyScene8::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene8::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
}

MyScene8::~MyScene8()
{
    logger.trace("Scene {} destructor called", title);
}
