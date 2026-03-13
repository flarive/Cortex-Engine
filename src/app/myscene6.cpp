#include "myscene6.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene6::MyScene6(string _title, App* _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .HDRSkyboxBlurStrength = 5.0f,
        .shadowIntensity = 1.0f,
        .iblDiffuseIntensity = 0.1f,
        .iblSpecularIntensity = 0.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene6::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, -16.0f, 8.0f } };
    auto camera1 = make_shared<FlyCamera>(100.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);






    // lights
    auto trsLight1 = Transform{};
    trsLight1.setLocalPosition({ -10.0f, 10.0f, 10.0f });
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(10.0f);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{};
    trsLight2.setLocalPosition({ 10.0f, 10.0f, 10.0f });
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(10.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{};
    trsLight3.setLocalPosition({ -10.0f, -10.0f, 10.0f });
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(10.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{};
    trsLight4.setLocalPosition({ 10.0f, -10.0f, 10.0f });
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(10.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);




    // helmet model
    auto helmetModel = make_shared<Model>("models/helmet/DamagedHelmet.glTF", false, true);
    auto trsHelmet = Transform(vec3(0.0f, -15.0f, -10.0f), vec3(8.0f), vec3(0.0f, 180.0f, 0.0f));
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


    ourText.setup(app->window, FONT_PATH, 28);
    ourText2.setup(app->window, FONT_PATH, 28);
    ourSprite.setup(app->window, "UI/cortex-logo.png");
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene6::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    //bool shiftPressed = (mods & GLFW_MOD_SHIFT);

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
}


void MyScene6::mouse_callback(double xposIn, double yposIn)
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

void MyScene6::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene6::gamepad_callback(const GLFWgamepadstate& state)
{
    getActiveCamera()->processJoystickMovement(state);

    //cout << "Left Stick X Axis: " << state.axes[0] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Left Stick Y Axis: " << state.axes[1] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Right Stick X Axis: " << state.axes[2] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Right Stick Y Axis: " << state.axes[3] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Left Trigger/L2: " << state.axes[4] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Right Trigger/R2: " << state.axes[5] << endl; // tested with PS4 controller connected via micro USB cable

    if (GLFW_PRESS == state.buttons[1])
    {
        cout << "Pressed" << endl;
    }
    else if (GLFW_RELEASE == state.buttons[0])
    {
        //cout << "Released" << endl;
    }
}

void MyScene6::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene6::update(Shader& shader)
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
    ourText.clean();
    ourText2.clean();
    ourSprite.clean();
}

void MyScene6::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing

    auto myHelmet = getEntityManager().findEntityByName("MyHelmet");
    if (myHelmet)
    {
        auto trs = myHelmet->getTransform();
        auto rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rot.y + rotation, rot.z));
        myHelmet->setTransform(trs);
    }

    rotation += deltaTime * 0.002f;
}

void MyScene6::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, vec3(1.0f));
    ourText2.draw(format("{} polys", polycount), app->width - 250.0f, 25.0f, 1.0f, vec3(1.0f));
    ourSprite.draw(vec2(40, app->height - 40), vec2(128.0f, -128.0f), 0.0f, vec3(1.0f));
}
