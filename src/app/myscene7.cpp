#include "myscene7.h"

using namespace std;
using namespace glm;
using namespace engine;


MyScene7::MyScene7(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
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

    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene7::before_init_hook()
{

}

void MyScene7::init()
{
    // cameras
    auto trsCamera1 = Transform{ { 0.0f, -16.0f, 2.0f } };
    auto camera1 = make_shared<FlyCamera>(100.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);



    // lights
    auto trsLight1 = Transform{ { -10.0f, -5.0f, -10.0f } };
    auto light1 = make_shared<PointLight>();
    light1->setIntensity(20.0f);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    auto trsLight2 = Transform{ { 10.0f, -5.0f, -10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->setIntensity(20.0f);
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{ { -10.0f, -25.0f, -10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->setIntensity(20.0f);
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{ { 10.0f, -25.0f, -10.0f } };
    auto light4 = make_shared<PointLight>();
    light4->setIntensity(20.0f);
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);










    auto sharedModel = make_shared<SharedModel>("models/helmet/DamagedHelmet.glTF", false, true);

    


    // sample flat entity hierarchy
    float offset = -15.0f;
    for (unsigned int i = 1; i <= 7; ++i)
    {
        auto trs = Transform{};
        trs.setLocalPosition({ offset, -22.0f, -10.0f });
        trs.setLocalScale(vec3(2.0f));
        trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });

        auto model = make_shared<Model>(sharedModel);

        auto entity = make_shared<Entity>(format("Child{}", i));
        entity->addComponent<TransformComponent>(trs);
        entity->addComponent<ModelComponent>(model);
        getEntityManager().addChild(entity);

        offset += 5.0f;
    }


    // sample nested entity hierarchy
    offset = -15.0f;
    shared_ptr<Entity> lastEntity = getEntityManager().getRootEntity();
    for (unsigned int i = 1; i < 8; ++i)
    {
        auto trs = Transform{};

        if (i == 1)
        {
            // parent
            trs.setLocalPosition({ offset, -12.0f, -10.0f });
            trs.setLocalScale(vec3(2.0f));
            trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });

            offset = 0.0f;
        }
        else
        {
            // childs
            trs.setLocalPosition({ offset, 0.0f, 0.0f });
            trs.setLocalScale(vec3(1.0f));
            trs.setLocalRotation({ 0.0f, 0.0f, 0.0f });
        }

        auto model = make_shared<Model>(sharedModel);

        auto entity = make_shared<Entity>(format("NestedChild{}", i));
        entity->addComponent<TransformComponent>(trs);
        entity->addComponent<ModelComponent>(model);
        getEntityManager().addChild(lastEntity, entity);

        if (i == 1)
            lastEntity = lastEntity->children.back();

        offset -= 2.5f;
    }




    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/rusted_metal_diffuse.jpg",
        "textures/rusted_metal_specular.jpg"), UvMapping(1.0f));

    auto trsPlane = Transform(vec3(0.0f, -10.0f, -10.0f), vec3(10.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // cube
    auto myCube = make_shared<Cube>(2.0f);
    myCube->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/container2_diffuse.png",
        "textures/container2_specular.png"));

    auto trsCube = Transform(vec3(-3.0f, -17.0f, -10.0f), vec3(2.0f), vec3(90.0f, 0.0f, 0.0f));
    auto entityCube = make_shared<Entity>("MyCube");
    entityCube->addComponent<TransformComponent>(trsCube);
    entityCube->addComponent<PrimitiveComponent>(myCube);
    getEntityManager().addChild(entityCube);


    // sphere
    auto mySphere = make_shared<Sphere>();
    mySphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/uv_mapper.jpg"));

    auto trsSphere = Transform(vec3(3.0f, -17.0f, -10.0f), vec3(2.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entitySphere = make_shared<Entity>("MySphere");
    entitySphere->addComponent<TransformComponent>(trsSphere);
    entitySphere->addComponent<PrimitiveComponent>(mySphere);
    getEntityManager().addChild(entitySphere);






    textFPSCount.setup(getApp()->window, FONT_PATH, 28);
    textPolyCount.setup(getApp()->window, FONT_PATH, 28);
    textMeshCount.setup(getApp()->window, FONT_PATH, 28);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 28);
    ourSprite.setup(getApp()->window, "UI/cortex-logo.png");
}


void MyScene7::after_init_hook()
{
    // do something here if needed
}




// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void MyScene7::key_callback(int key, int scancode, int action, int mods)
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

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        auto trs = Transform{};
        trs.setLocalPosition({ offset_dynamic, -32.0f, -10.0f });
        trs.setLocalScale(vec3(2.0f));
        trs.setLocalRotation({ 0.0f, 180.0f, 0.0f });

        auto model = make_shared<Model>("models/helmet/DamagedHelmet.glTF", false, true);
        auto entity = make_shared<Entity>(format("Child{}", 88));
        getEntityManager().addChild(entity);

        offset_dynamic += 5.0f;
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        getEntityManager().remove("Child88");
    }
}


void MyScene7::mouse_callback(double xposIn, double yposIn)
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

void MyScene7::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene7::gamepad_callback(const GLFWgamepadstate& state)
{
    getActiveCamera()->processJoystickMovement(state);

    //cout << "Left Stick X Axis: " << state.axes[0] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Left Stick Y Axis: " << state.axes[1] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Right Stick X Axis: " << state.axes[2] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Right Stick Y Axis: " << state.axes[3] << endl; // tested with PS4 controller connected via micro USB cable
    //cout << "Left Trigger/L2: " << state.axes[4] << endl; // tested with PS4 controller connected via micro USB cable
    //cout <<
    // "Right Trigger/R2: " << state.axes[5] << endl; // tested with PS4 controller connected via micro USB cable

    if (GLFW_PRESS == state.buttons[1])
    {
        cout << "Pressed" << endl;
    }
    else if (GLFW_RELEASE == state.buttons[0])
    {
        //cout << "Released" << endl;
    }
}

void MyScene7::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene7::update(Shader& shader)
{
    (void)shader;   //Do nothing

    // Normalize rotation to [0, 360)
    rotation = fmod(rotation, 360.0f);
    if (rotation < 0) {
        rotation += 360.0f;
    }

    auto child2 = getEntityManager().findEntityByName("Child1");
    if (child2)
    {
        auto& trs = child2->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        child2->setTransform(trs);
    }


    auto mySphere = getEntityManager().findEntityByName("MySphere");
    if (mySphere)
    {
        auto& trs = mySphere->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        mySphere->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene7::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);
    ourSprite.draw(vec2(50, 50), vec2(50.0f), 0.0f, Colors::White);
}

void MyScene7::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    ourSprite.clean();
}
