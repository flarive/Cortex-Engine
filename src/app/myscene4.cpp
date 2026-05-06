#include "myscene4.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene4::MyScene4(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .shadowIntensity = 0.8f,
        .shadowMapsBlur = 11.0f,
        .iblDiffuseIntensity = 0.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene4::init()
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
    light1->setCutoff(12.5f);
    light1->setOuterCutoff(15.0f);
    light1->setTarget(vec3(0.0f, 0.0f, -5.0f));
    light1->setAmbientColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{ { -10.0f, 10.0f, 10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(290.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);


    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<PBRMaterial>(Color(0.2f),
        "textures/pbr/planks/albedo.jpg",
        "textures/pbr/planks/normal.jpg",
        "textures/pbr/planks/metallic.jpg",
        "textures/pbr/planks/roughness.jpg",
        "textures/pbr/planks/ao.jpg",
        ""), UvMapping(1.0f));
    auto trsPlane = Transform(vec3(0.0f, -11.0f, -16.0f), vec3(12.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // cushion model
    shared_ptr<Model> cushionModel = make_shared<Model>("models/cushion/cushion.obj");
    auto trsCushion = Transform(vec3(0.0f, -9.85f + 1.0f, -10.0f), vec3(1.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityCushion = make_shared<Entity>("MyCushion");
    entityCushion->addComponent<TransformComponent>(trsCushion);
    entityCushion->addComponent<ModelComponent>(cushionModel);
    getEntityManager().addChild(entityCushion);

    ourText.setup(getApp()->window, FONT_PATH, 28);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene4::key_callback(int key, int scancode, int action, int mods)
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


void MyScene4::mouse_callback(double xposIn, double yposIn)
{
    Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode || show_demo_window)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    getActiveCamera()->processMouseMovement(xoffset, yoffset);
}

void MyScene4::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene4::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene4::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(getApp()->window, FONT_PATH, 28);
}

void MyScene4::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene4::updateUI()
{
    drawUI();
}

void MyScene4::clean()
{
    // clean up any resources
    ourText.clean();
}

void MyScene4::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        auto& trs = myCushion->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        myCushion->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene4::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
}
