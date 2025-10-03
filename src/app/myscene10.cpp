#include "myscene10.h"

MyScene10::MyScene10(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}



void MyScene10::init()
{
    auto trsCamera1 = engine::Transform{ { 0.0f, 5.0f, 3.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>();
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = std::make_shared<engine::Entity>("Camera1");
    entityCamera1->addComponent<engine::TransformComponent>(trsCamera1);
    entityCamera1->addComponent<engine::CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);






    // lights
    auto trsLight1 = engine::Transform{};
    trsLight1.setLocalPosition({ -10.0f, 10.0f, 10.0f });
    auto light1 = std::make_shared<engine::PointLight>(0);
    light1->intensity = 50.0f;
    auto entityLight1 = std::make_shared<engine::Entity>("Light1");
    entityLight1->addComponent<engine::TransformComponent>(trsLight1);
    entityLight1->addComponent<engine::LightComponent>(light1);
    getEntityManager().addChild(entityLight1);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene10::key_callback(int key, int scancode, int action, int mods)
{
    engine::Scene::key_callback(key, scancode, action, mods);

    //// Detect Shift key state
    ////bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    //if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);
    //    getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
    //}



    //if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);
    //    getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
    //}


    //if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);
    //}

    //if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
    //}
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
    //helmetModel.clean();
}

void MyScene10::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene10::drawUI()
{
    // render HUD / UI
}
