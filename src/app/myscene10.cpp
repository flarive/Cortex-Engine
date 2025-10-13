#include "myscene10.h"

#include <random>

MyScene10::MyScene10(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 0.0f,
        .iblSpecularIntensity = 0.0f
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}



void MyScene10::init()
{
    // camera
    auto trsCamera1 = engine::Transform{ { 0.0f, 5.0f, 3.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>();
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = std::make_shared<engine::Entity>("Camera1");
    entityCamera1->addComponent<engine::TransformComponent>(trsCamera1);
    entityCamera1->addComponent<engine::CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // lights
    std::uniform_real_distribution<GLfloat> random_floats(0.0f, 1.0f);
    typedef std::chrono::high_resolution_clock myclock;
    auto seed = myclock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::function<float(void)> fn = [&random_floats, &generator] { return random_floats(generator); };
    for (int i = 0; i < NUM_AREA_LIGHTS; i++)
    {
        float x = fn(); x = (x > 0.5f) ? x : -x;
        float z = fn(); z = (z > 0.5f) ? z : -z;


        // plane
        //auto myPlane = std::make_shared<engine::Plane>();
        //myPlane->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f), engine::Colors::YellowGreen, engine::Colors::Crimson));

        auto trsLight = engine::Transform{ { glm::vec3(x, 0.0f, z) * 8.f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, fn() * 360.0f, 0.0f} };
        auto light = std::make_shared<engine::AreaLight>(); //myPlane
        light->color = glm::vec3(fn(), fn(), fn());
        light->roughness = 0.5f;
        light->intensity = 1.0f;
        light->twoSided = false;
        auto entityLight = std::make_shared<engine::Entity>(std::format("AreaLight{}", i + 1));
        entityLight->addComponent<engine::TransformComponent>(trsLight);
        entityLight->addComponent<engine::LightComponent>(light);
        getEntityManager().addChild(entityLight);
    }

    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f), "textures/concreteTexture.png"), engine::UvMapping(6.0f));
    //myPlane->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.1f), engine::Colors::DarkGrey, engine::Colors::Crimson, 1.0f), engine::UvMapping(6.0f));
    auto trsPlane = engine::Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(16.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityPlane = std::make_shared<engine::Entity>("MyPlane");
    entityPlane->addComponent<engine::TransformComponent>(trsPlane);
    entityPlane->addComponent<engine::PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene10::key_callback(int key, int scancode, int action, int mods)
{
    engine::Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);
        getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);
        getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
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
}

void MyScene10::mouse_callback(double xposIn, double yposIn)
{
    engine::Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode)
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
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene10::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene10::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene10::update(engine::Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene10::incrementRoughness(float step)
{
    static glm::vec3 color = engine::Colors::SlateGray;
    static float roughness = 0.5f;
    roughness += step;
    roughness = glm::clamp(roughness, 0.0f, 1.0f);

    auto areaLights = getEntityManager().findEntitiesOfType<engine::AreaLight>();
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

    auto areaLights = getEntityManager().findEntitiesOfType<engine::AreaLight>();
    if (areaLights.size() > 0)
    {
        for (const auto& areaLight : areaLights)
        {
            areaLight->intensity = intensity;
        }
    }
}

void MyScene10::switchTwoSided(bool doSwitch)
{
    static bool twoSided = true;
    //if (doSwitch) twoSided = !twoSided;

    twoSided = doSwitch;

    auto areaLights = getEntityManager().findEntitiesOfType<engine::AreaLight>();
    if (areaLights.size() > 0)
    {
        for (const auto& areaLight : areaLights)
        {
            areaLight->twoSided = twoSided;
        }
    }
}


void MyScene10::updateUI()
{
    drawUI();
}

void MyScene10::clean()
{
    // clean up any resources
}

void MyScene10::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene10::drawUI()
{
    // render HUD / UI
}
