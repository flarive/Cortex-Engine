#include "myscene13.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene13::MyScene13(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "",
        .HDRSkyboxBlurStrength = 0.0f,
        .enableShadows = true,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048
    })
{
    // my application specific state gets initialized here

    if (auto appPtr = app.lock()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}


void MyScene13::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 0.1f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // light
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
    myPlane->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"), UvMapping(6.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(8.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);




    // particle systems
    auto myParticleSystem = make_shared<ParticleSystem>(500, 100, 0.15f, 2.0f, -0.25f);
    myParticleSystem->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/particles/rect.jpg"), UvMapping(1.0f));
    auto trsParticleSystem = Transform(vec3(-1.5f, -0.5f, 0.0f), vec3(0.4f), vec3(0.0f, 0.0f, 0.0f));
    auto entityParticleSystem = make_shared<Entity>("MyParticleSystem1");
    entityParticleSystem->addComponent<TransformComponent>(trsParticleSystem);
    entityParticleSystem->addComponent<ParticleSystemComponent>(myParticleSystem);
    getEntityManager().addChild(entityParticleSystem);

    auto myParticleSystem2 = make_shared<ParticleSystem>(500, 100, 0.65f, 2.0f, -0.25f);
    myParticleSystem2->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/particles/fire.png"), UvMapping(1.0f));
    auto trsParticleSystem2 = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(0.4f), vec3(0.0f, 0.0f, 0.0f));
    auto entityParticleSystem2 = make_shared<Entity>("MyParticleSystem2");
    entityParticleSystem2->addComponent<TransformComponent>(trsParticleSystem2);
    entityParticleSystem2->addComponent<ParticleSystemComponent>(myParticleSystem2);
    getEntityManager().addChild(entityParticleSystem2);

    auto myParticleSystem3 = make_shared<ParticleSystem>(500, 25, 0.20f, 2.0f, -0.25f);
    myParticleSystem3->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/particles/purple.png"), UvMapping(1.0f));
    auto trsParticleSystem3 = Transform(vec3(1.5f, -0.5f, 0.0f), vec3(0.4f), vec3(0.0f, 0.0f, 0.0f));
    auto entityParticleSystem3 = make_shared<Entity>("MyParticleSystem3");
    entityParticleSystem3->addComponent<TransformComponent>(trsParticleSystem3);
    entityParticleSystem3->addComponent<ParticleSystemComponent>(myParticleSystem3);
    getEntityManager().addChild(entityParticleSystem3);




    textFPSCount.setup(getApp()->window, FONT_PATH, 28);

    textPolyCount.setup(getApp()->window, FONT_PATH, 28);
    textMeshCount.setup(getApp()->window, FONT_PATH, 28);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 28);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 28);
    textTotalCount.setup(getApp()->window, FONT_PATH, 28);
}


void MyScene13::key_callback(int key, int scancode, int action, int mods)
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

void MyScene13::mouse_callback(double xposIn, double yposIn)
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

void MyScene13::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene13::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene13::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene13::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene13::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}

void MyScene13::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}
