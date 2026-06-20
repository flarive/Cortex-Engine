#include "myscene1.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene1::MyScene1(const string& _title, std::weak_ptr<App> _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::BlinnPhong,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "",
        .HDRSkyboxBlurStrength = 0.0f,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048
    })
{
    // my application specific state gets initialized here
    if (auto appPtr = getApp()) {
        lastX = appPtr->width / 2.0f;
        lastY = appPtr->height / 2.0f;
    }
}

void MyScene1::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 0.1f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(3.0f);
    light1->setCutoff(12.0f);
    light1->setOuterCutoff(48.0f);
    light1->setTarget(vec3(0.0f, 0.0f, 0.0f));
    light1->setAmbientColor(Color(1.0f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);


    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"), UvMapping(6.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(8.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // billboard
    auto myBillboard = make_shared<Billboard>();
    myBillboard->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/grass.png"), UvMapping(1.0f));
    auto trsBillboard = Transform(vec3(0.0f, -0.18f, 1.35f), vec3(0.7f));
    auto entityBillboard = make_shared<Entity>("MyBillboard");
    entityBillboard->addComponent<TransformComponent>(trsBillboard);
    entityBillboard->addComponent<PrimitiveComponent>(myBillboard);
    getEntityManager().addChild(entityBillboard);


    // cube
    auto myCube = make_shared<Cube>(2.0f);
    myCube->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"));
    auto trsCube = Transform(vec3(0.0f, -0.35f, 0.0f), vec3(0.15f));
    auto entityCube = make_shared<Entity>("MyCube");
    entityCube->addComponent<TransformComponent>(trsCube);
    entityCube->addComponent<PrimitiveComponent>(myCube);
    getEntityManager().addChild(entityCube);


    // cylinder
    auto myCylinder = make_shared<Cylinder>(0.1f, 0.3f);
    myCylinder->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"), UvMapping(1.0f));
    auto trsCylinder = Transform(vec3(0.5f, -0.35f, 0.0f));
    auto entityCylinder = make_shared<Entity>("MyCylinder");
    entityCylinder->addComponent<TransformComponent>(trsCylinder);
    entityCylinder->addComponent<PrimitiveComponent>(myCylinder);
    getEntityManager().addChild(entityCylinder);


    // cone
    auto myCone = make_shared<Cone>(0.1f, 0.3f);
    myCone->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"), UvMapping(1.0f));
    auto trsCone = Transform(vec3(1.0f, -0.35f, 0.0f));
    auto entityCone = make_shared<Entity>("MyCone");
    entityCone->addComponent<TransformComponent>(trsCone);
    entityCone->addComponent<PrimitiveComponent>(myCone);
    getEntityManager().addChild(entityCone);


    // sphere
    auto mySphere = make_shared<Sphere>();
    mySphere->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"), UvMapping(1.0f));
    auto trsSphere = Transform(vec3(1.5f, -0.35f, 0.0f), vec3(0.2f));
    auto entitySphere = make_shared<Entity>("MySphere");
    entitySphere->addComponent<TransformComponent>(trsSphere);
    entitySphere->addComponent<PrimitiveComponent>(mySphere);
    getEntityManager().addChild(entitySphere);


    // cushion model
    auto cushionModel = make_shared<Model>("models/cushion/cushion.glb");
    auto trsCushion = Transform(vec3(-0.5f, -0.35f, 0.0f), vec3(0.10f), vec3(0.0f, 45.0f, 0.0f));
    auto entityCushion = make_shared<Entity>("MyCushion");
    entityCushion->addComponent<TransformComponent>(trsCushion);
    entityCushion->addComponent<ModelComponent>(cushionModel);
    getEntityManager().addChild(entityCushion);


    // backpack model
    auto backpackModel = make_shared<Model>("models/backpack/backpack.glb");
    auto trsBackpack = Transform(vec3(-1.0f, -0.25f, 0.0f), vec3(0.12f), vec3(90.0f, 0.0f, 0.0f));
    auto entityBackpack = make_shared<Entity>("MyBackpack");
    entityBackpack->addComponent<TransformComponent>(trsBackpack);
    entityBackpack->addComponent<ModelComponent>(backpackModel);
    getEntityManager().addChild(entityBackpack);


    // cube outside camera frustrum
    auto myCube2 = make_shared<Cube>();
    myCube2->setup(make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/uv_mapper.jpg"));
    auto trsCube2 = Transform(vec3(-3.0f, -0.35f, 0.0f), vec3(0.15f));
    auto entityCube2 = make_shared<Entity>("MyCube2");
    entityCube2->addComponent<TransformComponent>(trsCube2);
    entityCube2->addComponent<PrimitiveComponent>(myCube2);
    getEntityManager().addChild(entityCube2);


    // skybox
    // non‑owning observer pointer. You did not create it with new, and you do not own it
    // (If you didn’t new it, you don’t delete it)
    BlinnPhongRenderer* renderer = dynamic_cast<BlinnPhongRenderer*>(getRenderer());
    if (renderer)
    {
        vector<string> faces
        {
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
        };
        renderer->setSkybox(faces);
    }

    

    textFPSCount.setup(getApp()->window, FONT_PATH, 20);
    textElapsedFramesCount.setup(getApp()->window, FONT_PATH, 20);

    textPolyCount.setup(getApp()->window, FONT_PATH, 20);
    textMeshCount.setup(getApp()->window, FONT_PATH, 20);
    textPrimitiveCount.setup(getApp()->window, FONT_PATH, 20);

    textDrawnCount.setup(getApp()->window, FONT_PATH, 20);
    textTotalCount.setup(getApp()->window, FONT_PATH, 20);
}


void MyScene1::key_callback(int key, int scancode, int action, int mods)
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

void MyScene1::mouse_callback(double xposIn, double yposIn)
{
    Scene::mouse_callback(xposIn, yposIn);

    if (is_editor_mode || show_demo_window)
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

void MyScene1::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene1::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene1::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene1::update(Shader& shader)
{
    (void)shader;   //Do nothing

    // Normalize rotation to [0, 360)
    rotation = fmod(rotation, 360.0f);
    if (rotation < 0) {
        rotation += 360.0f;
    }

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        auto& trs = myCushion->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        myCushion->setTransform(trs);
    }

    auto myBackpack = getEntityManager().findEntityByName("MyBackpack");
    if (myBackpack)
    {
        auto& trs = myBackpack->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        myBackpack->setTransform(trs);
    }

    auto myCylinder = getEntityManager().findEntityByName("MyCylinder");
    if (myCylinder)
    {
        auto& trs = myCylinder->getTransform();
        auto& rot = trs.getLocalRotation();
        trs.setLocalRotation(vec3(rot.x, rotation, rot.z));
        myCylinder->setTransform(trs);
    }

    rotation += deltaTime * 10.0f;
}

void MyScene1::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);
    textElapsedFramesCount.draw(format("{} frames", getTotalElapsedFrames()), 250.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), getApp()->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), getApp()->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), getApp()->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);
}

void MyScene1::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textElapsedFramesCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

MyScene1::~MyScene1()
{
    logger.trace("Scene {} destructor called", title);
}
