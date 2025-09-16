#include "myscene4.h"

MyScene4::MyScene4(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .shadowIntensity = 1.5f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}

void MyScene4::init()
{
    // cameras
    auto trsCamera1 = engine::Transform{ {0.0f, -8.0f, 2.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>();
    camera1->zoom = 25.0f;
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = std::make_shared<engine::Entity>("Camera1");
    entityCamera1->addComponent<engine::TransformComponent>(trsCamera1);
    entityCamera1->addComponent<engine::CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);

    auto trsCamera2 = engine::Transform{ { 0.0f, -10.0f, 2.0f } };
    auto camera2 = std::make_shared<engine::FlyCamera>();
    camera2->zoom = 15.0f;
    camera2->movementSpeed = 10.0f;
    auto entityCamera2 = std::make_shared<engine::Entity>("Camera2");
    entityCamera2->addComponent<engine::TransformComponent>(trsCamera2);
    entityCamera2->addComponent<engine::CameraComponent>(camera2);
    getEntityManager().addChild(entityCamera2);


    this->setActiveCamera(1);


    // lights
    auto trsLight1 = engine::Transform{ { 0.0f, 8.0f, 0.0f } };
    auto light1 = std::make_shared<engine::SpotLight>(0);
    light1->intensity = 20.0f;
    light1->cutoff = 12.5f;
    light1->outerCutoff = 17.5f;
    light1->target = glm::vec3(0.0f, 0.0f, -5.0f);
    light1->ambientColor = engine::Color(0.1f, 0.1f, 0.1f, 1.0f);
    auto entityLight1 = std::make_shared<engine::Entity>("Light1");
    entityLight1->addComponent<engine::TransformComponent>(trsLight1);
    entityLight1->addComponent<engine::LightComponent>(light1);
    getEntityManager().addChild(entityLight1);




    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::PBRMaterial>(engine::Color(0.2f),
        "textures/pbr/planks/albedo.jpg",
        "",
        "textures/pbr/planks/normal.jpg",
        "textures/pbr/planks/metallic.jpg",
        "textures/pbr/planks/roughness.jpg",
        "textures/pbr/planks/ao.jpg",
        ""), engine::UvMapping(1.0f));
    auto trsPlane = engine::Transform(glm::vec3(0.0f, -11.0f, -10.0f), glm::vec3(8.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityPlane = std::make_shared<engine::Entity>("MyPlane");
    entityPlane->addComponent<engine::TransformComponent>(trsPlane);
    entityPlane->addComponent<engine::PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // cushion model
    std::shared_ptr<engine::Model> cushionModel = std::make_shared<engine::Model>("models/cushion/cushion.obj");
    auto trsCushion = engine::Transform(glm::vec3(0.0f, -9.85f + 2.0f, -10.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCushion = std::make_shared<engine::Entity>("MyCushion");
    entityCushion->addComponent<engine::TransformComponent>(trsCushion);
    entityCushion->addComponent<engine::ModelComponent>(cushionModel);
    getEntityManager().addChild(entityCushion);

    ourText.setup(app->window, FONT_PATH, 28);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene4::key_callback(int key, int scancode, int action, int mods)
{
    engine::Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::YAW_DOWN, deltaTime);
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::LEFT, deltaTime);

    if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::YAW_UP, deltaTime);
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::RIGHT, deltaTime);



    if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::PITCH_UP, deltaTime);
    else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::FORWARD, deltaTime);

    if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::PITCH_DOWN, deltaTime);
    else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(engine::BACKWARD, deltaTime);
}


void MyScene4::mouse_callback(double xposIn, double yposIn)
{
    engine::Scene::mouse_callback(xposIn, yposIn);

    if (show_window)
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
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene4::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene4::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene4::update(engine::Shader& shader)
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
    /*ourPlane.clean();
    cushionModel.clean();*/
}

void MyScene4::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        myCushion->getTransform().setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
    }

    rotation += deltaTime * 10.0f;
}

void MyScene4::drawUI()
{
    // render HUD / UI
    ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}
