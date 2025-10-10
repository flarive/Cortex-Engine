#include "myscene8.h"

MyScene8::MyScene8(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::BlinnPhong,
        .shadowIntensity = 5.0f
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}

void MyScene8::init()
{
    // cameras
    auto trsCamera1 = engine::Transform{ { 0.0f, 0.0f, 3.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>();
    camera1->zoom = 25.0f;
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = std::make_shared<engine::Entity>("Camera1");
    entityCamera1->addComponent<engine::TransformComponent>(trsCamera1);
    entityCamera1->addComponent<engine::CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);






    // light
    auto trsLight1 = engine::Transform{ {0.0f, 1.5f, 0.0f} };
    auto light1 = std::make_shared<engine::PointLight>();
    light1->intensity = 1.0f;
    light1->ambientColor = engine::Color(0.5f);
    auto entityLight1 = std::make_shared<engine::Entity>("Light1");
    entityLight1->addComponent<engine::TransformComponent>(trsLight1);
    entityLight1->addComponent<engine::LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto zzz = engine::ColorManager::hexToNormalizedRGB("#FFF1AD");
    auto zzz2 = engine::Color(0.5f);
    auto zzz3 = engine::Color(0.5f);

    //auto zzz = engine::Color(1.0f, 0.5f, 0.31f, 1.0f); //engine::ColorManager::hexToNormalizedRGB("#FFF1AD");
    //auto zzz2 = engine::Color(1.0f, 0.0f, 0.0f, 1.0f); // engine::ColorManager::hexToNormalizedRGB("#FFFF99");
    //auto zzz3 = engine::Color(0.0f, 1.0f, 0.0f, 1.0f); // engine::ColorManager::hexToNormalizedRGB("#00FFFF");


    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f), engine::UvMapping(6.0f));
    auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(2.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityPlane = std::make_shared<engine::Entity>("MyPlane");
    entityPlane->addComponent<engine::TransformComponent>(trsPlane);
    entityPlane->addComponent<engine::PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);



    // cube 1
    auto myCube1 = std::make_shared<engine::Cube>();
    myCube1->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube1 = engine::Transform(glm::vec3(-1.0f, -0.35f, -1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCube1 = std::make_shared<engine::Entity>("MyCube1");
    entityCube1->addComponent<engine::TransformComponent>(trsCube1);
    entityCube1->addComponent<engine::PrimitiveComponent>(myCube1);
    getEntityManager().addChild(entityCube1);


    // cube 2
    auto myCube2 = std::make_shared<engine::Cube>();
    myCube2->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

    auto trsCube2 = engine::Transform(glm::vec3(1.0f, -0.35f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCube2 = std::make_shared<engine::Entity>("MyCube2");
    entityCube2->addComponent<engine::TransformComponent>(trsCube2);
    entityCube2->addComponent<engine::PrimitiveComponent>(myCube2);
    getEntityManager().addChild(entityCube2);


    // cube 3
    auto myCube3 = std::make_shared<engine::Cube>();
    myCube3->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));
    auto trsCube3 = engine::Transform(glm::vec3(1.0f, -0.35f, -1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCube3 = std::make_shared<engine::Entity>("MyCube3");
    entityCube3->addComponent<engine::TransformComponent>(trsCube3);
    entityCube3->addComponent<engine::PrimitiveComponent>(myCube3);
    getEntityManager().addChild(entityCube3);


    // cube 4
    auto myCube4 = std::make_shared<engine::Cube>();
    myCube4->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

    auto trsCube4 = engine::Transform(glm::vec3(-1.0f, -0.35f, 1.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCube4 = std::make_shared<engine::Entity>("MyCube4");
    entityCube4->addComponent<engine::TransformComponent>(trsCube4);
    entityCube4->addComponent<engine::PrimitiveComponent>(myCube4);
    getEntityManager().addChild(entityCube4);


    // sphere
    auto mySphere = std::make_shared<engine::Sphere>();
    mySphere->setup(std::make_shared<engine::BlinnPhongMaterial>(zzz, zzz2, zzz3, 32.0f));

    auto trsSphere = engine::Transform(glm::vec3(0.0f, -0.35f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entitySphere = std::make_shared<engine::Entity>("MySphere");
    entitySphere->addComponent<engine::TransformComponent>(trsSphere);
    entitySphere->addComponent<engine::PrimitiveComponent>(mySphere);
    getEntityManager().addChild(entitySphere);



    ourText.setup(app->window, FONT_PATH, 28);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene8::key_callback(int key, int scancode, int action, int mods)
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


void MyScene8::mouse_callback(double xposIn, double yposIn)
{
    engine::Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode)
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
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene8::gamepad_callback(const GLFWgamepadstate& state)
{
    engine::Scene::gamepad_callback(state);
}

void MyScene8::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene8::update(engine::Shader& shader)
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
}

void MyScene8::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene8::drawUI()
{
    // render HUD / UI
    ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}
