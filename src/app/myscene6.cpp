#include "myscene6.h"

MyScene6::MyScene6(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
    {
        .method = engine::RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 5.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 0.1f,
        .iblSpecularIntensity = 0.0f
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene6::init()
{
    // cameras
    auto trsCamera1 = engine::Transform{ { 0.0f, -16.0f, 8.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>();
    camera1->zoom = 100.0f;
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = std::make_shared<engine::Entity>("Camera1");
    entityCamera1->addComponent<engine::TransformComponent>(trsCamera1);
    entityCamera1->addComponent<engine::CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);






    // lights
    auto trsLight1 = engine::Transform{};
    trsLight1.setLocalPosition({ -10.0f, 10.0f, 10.0f });
    auto light1 = std::make_shared<engine::PointLight>();
    light1->intensity = 50.0f;
    auto entityLight1 = std::make_shared<engine::Entity>("Light1");
    entityLight1->addComponent<engine::TransformComponent>(trsLight1);
    entityLight1->addComponent<engine::LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = engine::Transform{};
    trsLight2.setLocalPosition({ 10.0f, 10.0f, 10.0f });
    auto light2 = std::make_shared<engine::PointLight>();
    light2->intensity = 50.0f;
    auto entityLight2 = std::make_shared<engine::Entity>("Light2");
    entityLight2->addComponent<engine::TransformComponent>(trsLight2);
    entityLight2->addComponent<engine::LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = engine::Transform{};
    trsLight3.setLocalPosition({ -10.0f, -10.0f, 10.0f });
    auto light3 = std::make_shared<engine::PointLight>();
    light3->intensity = 50.0f;
    auto entityLight3 = std::make_shared<engine::Entity>("Light3");
    entityLight3->addComponent<engine::TransformComponent>(trsLight3);
    entityLight3->addComponent<engine::LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = engine::Transform{};
    trsLight4.setLocalPosition({ 10.0f, -10.0f, 10.0f });
    auto light4 = std::make_shared<engine::PointLight>();
    light4->intensity = 50.0f;
    auto entityLight4 = std::make_shared<engine::Entity>("Light4");
    entityLight4->addComponent<engine::TransformComponent>(trsLight4);
    entityLight4->addComponent<engine::LightComponent>(light4);
    getEntityManager().addChild(entityLight4);


    // helmet model
    auto helmetModel = std::make_shared<engine::Model>("models/helmet/DamagedHelmet.glTF", false, true);
    auto trsHelmet = engine::Transform(glm::vec3(0.0f, -15.0f, -10.0f), glm::vec3(8.0f), glm::vec3(0.0f, 180.0f, 0.0f));
    auto entityHelmet = std::make_shared<engine::Entity>("MyHelmet");
    entityHelmet->addComponent<engine::TransformComponent>(trsHelmet);
    entityHelmet->addComponent<engine::ModelComponent>(helmetModel);
    getEntityManager().addChild(entityHelmet);

    auto helmetMat = helmetModel->meshes[0].getMaterial();
    if (helmetMat)
    {
        helmetMat->setAmbientIntensity(5.0f);
        helmetMat->setEmissiveIntensity(5.0f);
    }


    ourText.setup(app->window, FONT_PATH, 28);
    ourText2.setup(app->window, FONT_PATH, 28);
    ourSprite.setup(app->window, "textures/awesomeface.png");
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene6::key_callback(int key, int scancode, int action, int mods)
{
    engine::Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
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


void MyScene6::mouse_callback(double xposIn, double yposIn)
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

void MyScene6::scroll_callback(double xoffset, double yoffset)
{
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene6::gamepad_callback(const GLFWgamepadstate& state)
{
    getActiveCamera()->processJoystickMovement(state);

    //std::cout << "Left Stick X Axis: " << state.axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Left Stick Y Axis: " << state.axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Stick X Axis: " << state.axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Stick Y Axis: " << state.axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Left Trigger/L2: " << state.axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
    //std::cout << "Right Trigger/R2: " << state.axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable

    if (GLFW_PRESS == state.buttons[1])
    {
        std::cout << "Pressed" << std::endl;
    }
    else if (GLFW_RELEASE == state.buttons[0])
    {
        //std::cout << "Released" << std::endl;
    }
}

void MyScene6::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene6::update(engine::Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene6::updateUI()
{
    drawUI();
}

void MyScene6::clean()
{
    // clean up any resources
    //helmetModel.clean();
}

void MyScene6::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing

    auto myHelmet = getEntityManager().findEntityByName("MyHelmet");
    if (myHelmet)
    {
        auto trs = myHelmet->getTransform();
        auto rot = trs.getLocalRotation();
        trs.setLocalRotation(glm::vec3(rot.x, rot.y + rotation, rot.z));
        myHelmet->setTransform(trs);
    }

    rotation += deltaTime * 0.002f;
}

void MyScene6::drawUI()
{
    // render HUD / UI
    ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    ourText2.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    ourSprite.draw(glm::vec2(50, app->height - 100), glm::vec2(50.0f, 50.0f), 0.0f, glm::vec3(1.0f));
}
