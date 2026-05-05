#include "myscene9.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene9::MyScene9(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    if (auto appPtr = app.lock()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}



void MyScene9::init()
{
    // camera
    auto trsCamera1 = Transform{ { 0.0f, 5.0f, 3.0f } };
    auto camera1 = make_shared<OrbitCamera>();
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // lights
    auto trsLight1 = Transform{};
    trsLight1.setLocalPosition({ -10.0f, 10.0f, -10.0f });
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(10.0f);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{};
    trsLight2.setLocalPosition({ 10.0f, 10.0f, -10.0f });
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(10.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{};
    trsLight3.setLocalPosition({ -10.0f, -10.0f, -10.0f });
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(10.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{};
    trsLight4.setLocalPosition({ 10.0f, -10.0f, -10.0f });
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(10.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);



    // helmet model
    shared_ptr<Model> helmetModel = make_shared<Model>("models/helmet/DamagedHelmet.glTF", false, true);
    auto trsHelmet = Transform(vec3(0.0f, 0.0f, 0.0f), vec3(2.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityHelmet = make_shared<Entity>("MyHelmet");
    entityHelmet->addComponent<TransformComponent>(trsHelmet);
    entityHelmet->addComponent<ModelComponent>(helmetModel);
    getEntityManager().addChild(entityHelmet);

    auto helmetMat = helmetModel->meshes[0].getMaterial();
    if (helmetMat)
    {
        helmetMat->setAmbientIntensity(5.0f);
        helmetMat->setEmissiveIntensity(5.0f);
    }


    ourText.setup(getApp()->window, FONT_PATH, 28);
    ourText2.setup(getApp()->window, FONT_PATH, 28);
    ourSprite.setup(getApp()->window, "textures/awesomeface.png");
    textMeshCount.setup(getApp()->window, FONT_PATH, 28);
    rect.setup(getApp()->window);

    button.setup(getApp()->window, FONT_PATH, 16);
    button.setPosition({ 400, 100 });
    button.setSize({ 100, 50 });
    button.setText("GOOO");
    button.setTextColor(Colors::White);
    button.setBorderThickness(1.0f);

    button.setColors(
        Color(0.0f, 0.0f, 0.0f, 0.9f),
        Color(1.0f, 0.3f, 0.3f, 1.0f),
        Color(0.1f, 0.1f, 1.0f, 1.0f),
        Color(1.0f, 1.0f, 1.0f, 1.0f)
    );


    button.onClick([this]{
        std::cout << "Button clicked!\n";
        });
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene9::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    //// Detect Shift key state
    ////bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    //if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(LEFT, deltaTime);
    //    getActiveCamera()->processKeyboard(YAW_DOWN, deltaTime);
    //}



    //if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(RIGHT, deltaTime);
    //    getActiveCamera()->processKeyboard(YAW_UP, deltaTime);
    //}


    //if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(FORWARD, deltaTime);
    //}

    //if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    //{
    //    getActiveCamera()->processKeyboard(BACKWARD, deltaTime);
    //}
}


void MyScene9::mouse_callback(double xposIn, double yposIn)
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

void MyScene9::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene9::gamepad_callback(const GLFWgamepadstate& state)
{
    (void)state;   //Do nothing
}

void MyScene9::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(getApp()->window, FONT_PATH, 28);
}

void MyScene9::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene9::updateUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
    ourText2.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    rect.draw(glm::vec2(50, 50), glm::vec2(300, 120), 0.0f, Color(0.0f, 0.0f, 0.0f, 0.5f), Colors::White, 0.02f);
    ourSprite.draw(vec2(50, 50), vec2(50.0f), 0.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);


    double mouseX, mouseY;
    glfwGetCursorPos(getApp()->window, &mouseX, &mouseY);

    bool mousePressed = glfwGetMouseButton(getApp()->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    button.update(mouseX, mouseY, mousePressed);
    button.draw();
}

void MyScene9::clean()
{
    // clean up any resources
    ourText.clean();
    ourText2.clean();
    ourSprite.clean();
    textMeshCount.clean();
}
