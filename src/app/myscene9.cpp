#include "myscene9.h"

MyScene9::MyScene9(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
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



void MyScene9::init()
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


    auto trsLight2 = engine::Transform{};
    trsLight2.setLocalPosition({ 10.0f, 10.0f, 10.0f });
    auto light2 = std::make_shared<engine::PointLight>(1);
    light2->intensity = 50.0f;
    auto entityLight2 = std::make_shared<engine::Entity>("Light2");
    entityLight2->addComponent<engine::TransformComponent>(trsLight2);
    entityLight2->addComponent<engine::LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = engine::Transform{};
    trsLight3.setLocalPosition({ -10.0f, -10.0f, 10.0f });
    auto light3 = std::make_shared<engine::PointLight>(2);
    light3->intensity = 50.0f;
    auto entityLight3 = std::make_shared<engine::Entity>("Light3");
    entityLight3->addComponent<engine::TransformComponent>(trsLight3);
    entityLight3->addComponent<engine::LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = engine::Transform{};
    trsLight4.setLocalPosition({ 10.0f, -10.0f, 10.0f });
    auto light4 = std::make_shared<engine::PointLight>(3);
    light4->intensity = 50.0f;
    auto entityLight4 = std::make_shared<engine::Entity>("Light4");
    entityLight4->addComponent<engine::TransformComponent>(trsLight4);
    entityLight4->addComponent<engine::LightComponent>(light4);
    getEntityManager().addChild(entityLight4);



    // helmet model
    std::shared_ptr<engine::Model> helmetModel = std::make_shared<engine::Model>("models/helmet/DamagedHelmet.glTF", false, true);
    auto trsHelmet = engine::Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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
    textMeshCount.setup(app->window, FONT_PATH, 28);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene9::key_callback(int key, int scancode, int action, int mods)
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


void MyScene9::mouse_callback(double xposIn, double yposIn)
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

void MyScene9::scroll_callback(double xoffset, double yoffset)
{
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene9::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene9::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene9::update(engine::Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene9::updateUI()
{
    drawUI();
}

void MyScene9::clean()
{
    // clean up any resources
    //helmetModel.clean();
}

void MyScene9::drawScene(engine::Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene9::drawUI()
{
    // render HUD / UI
    ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    ourText2.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    ourSprite.draw(glm::vec2(50, app->height - 100), glm::vec2(50.0f, 50.0f), 0.0f, glm::vec3(1.0f));
    textMeshCount.draw(std::format("{} meshes", (int)meshcount), app->width - 450.0f, 25.0f, 1.0f, glm::vec3(1.0f));
}
