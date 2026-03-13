#include "myscene5.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene5::MyScene5(string _title, App* _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/blue_photo_studio_2k.hdr",
        .shadowIntensity = 0.9f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}

void MyScene5::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, -8.0f, 2.0f } };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);





    // lights
    auto trsLight1 = Transform{ { 0.0f, 6.0f, 0.0f } };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(50.0f);
    light1->setCutoff(12.5f);
    light1->setOuterCutoff(16.0f);
    light1->setTarget(vec3(0.0f, 0.0f, -5.0f));
    light1->setAmbientColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{ { -5.0f, -5.0f, -10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(40.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);

    auto trsLight3 = Transform{ { 5.0f, -5.0f, -10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(40.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);

    auto trsLight4 = Transform{ { 0.0f, -5.0f, -5.0f } };
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(40.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);


    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<PBRMaterial>(Color(0.2f),
        "textures/pbr/planks/albedo.jpg",
        "textures/pbr/planks/normal.jpg",
        "textures/pbr/planks/metallic.jpg",
        "textures/pbr/planks/roughness.jpg",
        "textures/pbr/planks/ao.jpg",
        ""), UvMapping(1.0f));

    auto trsPlane = Transform(vec3(0.0f, -11.0f, -16.0f), vec3(10.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // buddha model
    shared_ptr<Model> buddhaModel = make_shared<Model>("models/buddha/buddha1.obj");
    auto trsBuddha = Transform(vec3(0.0f, -11.0f + 1.0f, -10.0f), vec3(0.5f), vec3(0.0f, 0.0f, 0.0f));
    auto entityBuddha = make_shared<Entity>("MyBuddha");
    entityBuddha->addComponent<TransformComponent>(trsBuddha);
    entityBuddha->addComponent<ModelComponent>(buddhaModel);
    getEntityManager().addChild(entityBuddha);

    ourText.setup(app->window, FONT_PATH, 28);
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene5::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

    if (shiftPressed && key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(YAW_DOWN, deltaTime);
    else if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(LEFT, deltaTime);

    if (shiftPressed && key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(YAW_UP, deltaTime);
    else if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(RIGHT, deltaTime);



    if (shiftPressed && key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(PITCH_UP, deltaTime);
    else if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(FORWARD, deltaTime);

    if (shiftPressed && key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(PITCH_DOWN, deltaTime);
    else if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
        getActiveCamera()->processKeyboard(BACKWARD, deltaTime);
}


void MyScene5::mouse_callback(double xposIn, double yposIn)
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

void MyScene5::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene5::gamepad_callback(const GLFWgamepadstate& state)
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

void MyScene5::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene5::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene5::updateUI()
{
    drawUI();
}

void MyScene5::clean()
{
    // clean up any resources
    ourText.clean();
}

void MyScene5::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing

    auto myBuddha = getEntityManager().findEntityByName("MyBuddha");
    if (myBuddha)
    {
        auto trs = myBuddha->getTransform();
        trs.setLocalRotation(vec3(0.0f, rotation, 0.0f));
        myBuddha->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene5::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, vec3(0.5, 0.8f, 0.2f));
}
