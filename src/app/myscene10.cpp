#include "myscene10.h"

#include <random>

MyScene10::MyScene10(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::BlinnPhong
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

        auto trsLight = engine::Transform{ { 0.0f, 0.0f, 0.0f } };
        auto light = std::make_shared<engine::AreaLight>();
        light->offset = glm::vec3(x, 0.0f, z) * 8.f;
        light->yRotation = fn() * glm::two_pi<float>();
        light->color = glm::vec3(fn(), fn(), fn());
        light->roughness = 2.0f;
        light->intensity = 100.0f;
        light->twoSided = true;
        auto entityLight = std::make_shared<engine::Entity>(std::format("AreaLight{}", i + 1));
        entityLight->addComponent<engine::TransformComponent>(trsLight);
        entityLight->addComponent<engine::LightComponent>(light);
        getEntityManager().addChild(entityLight);
    }

    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/concreteTexture.png"), engine::UvMapping(6.0f));
    auto trsPlane = engine::Transform(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(16.0f), glm::vec3(90.0f, 0.0f, 0.0f));
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

    //// Detect Shift key state
    //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

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
