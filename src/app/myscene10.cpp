#include "myscene10.h"

#include <random>

using namespace std;
using namespace glm;
using namespace engine;

MyScene10::MyScene10(const string& _title, App* _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 0.0f,
        .iblSpecularIntensity = 0.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}



void MyScene10::init()
{
    // camera
    auto trsCamera1 = Transform{ { 0.0f, 1.0f, 0.0f } };
    auto camera1 = make_shared<FlyCamera>();
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // lights
    uniform_real_distribution<GLfloat> random_floats(0.0f, 1.0f);
    typedef chrono::high_resolution_clock myclock;
    auto seed = static_cast<unsigned int>(myclock::now().time_since_epoch().count());
    default_random_engine generator(seed);
    function<float(void)> fn = [&random_floats, &generator] { return random_floats(generator); };
    for (int i = 0; i < NUM_AREA_LIGHTS; i++)
    {
        float x = fn(); x = (x > 0.5f) ? x : -x;
        float z = fn(); z = (z > 0.5f) ? z : -z;

        auto trsLight = Transform{ { vec3(x, 0.0f, z) * 8.f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, fn() * 360.0f, 0.0f} };
        auto light = make_shared<AreaLight>();
        light->color = vec3(fn(), fn(), fn());
        light->roughness = 0.5f;
        light->setIntensity(1.0f);
        light->twoSided = false;
        auto entityLight = make_shared<Entity>(format("AreaLight{}", i + 1));
        entityLight->addComponent<TransformComponent>(trsLight);
        entityLight->addComponent<LightComponent>(light);
        getEntityManager().addChild(entityLight);
    }

    // ground
    auto myPlane = make_shared<Plane>(false);
    myPlane->setup(make_shared<PBRMaterial>(Color(10.0f), "textures/concrete_diffuse.png", "textures/concrete_normal.png"), UvMapping(6.0f));
    auto trsPlane = Transform(vec3(0.0f, 0.2f, 0.0f), vec3(16.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene10::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

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

    if (shiftPressed && key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementRoughness(-0.1f);
    else if (key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementRoughness(0.1f);

    if (shiftPressed && key == GLFW_KEY_I && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementLightIntensity(-0.1f);
    else if (key == GLFW_KEY_I && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementLightIntensity(0.1f);

    if (shiftPressed && key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        switchTwoSided(false);
    else if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        switchTwoSided(true);

    if (key == GLFW_KEY_O && (action == GLFW_REPEAT || action == GLFW_PRESS))
        playOggFile();
}
    
void MyScene10::mouse_callback(double xposIn, double yposIn)
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

void MyScene10::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene10::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene10::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene10::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene10::incrementRoughness(float step)
{
    static vec3 color = Colors::SlateGray;
    static float roughness = 0.5f;
    roughness += step;
    roughness = glm::clamp(roughness, 0.0f, 1.0f);

    auto areaLights = getEntityManager().findEntitiesOfType<AreaLight>();
    if (areaLights.size() > 0)
    {
        for (const auto& areaLight : areaLights)
        {
            areaLight->roughness = roughness;
        }
    }
}

void MyScene10::incrementLightIntensity(float step)
{
    static float intensity = 1.0f;
    intensity += step;
    intensity = glm::clamp(intensity, 0.0f, 100.0f);

    auto areaLights = getEntityManager().findEntitiesOfType<AreaLight>();
    if (areaLights.size() > 0)
    {
        for (const auto& areaLight : areaLights)
        {
            areaLight->setIntensity(intensity);
        }
    }
}

void MyScene10::switchTwoSided(bool doSwitch)
{
    static bool twoSided = true;
    //if (doSwitch) twoSided = !twoSided;

    twoSided = doSwitch;

    auto areaLights = getEntityManager().findEntitiesOfType<AreaLight>();
    if (areaLights.size() > 0)
    {
        for (const auto& areaLight : areaLights)
        {
            areaLight->twoSided = twoSided;
        }
    }
}

void MyScene10::playOggFile()
{
    AudioManager& audio = getAudioManager();
    audio.loadOgg("ogg1", "sounds/Example.ogg");
    audio.play("ogg1");
}

void MyScene10::updateUI()
{
    drawUI();
}

void MyScene10::clean()
{
    // clean up any resources
}

void MyScene10::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene10::drawUI()
{
    // render HUD / UI
}
