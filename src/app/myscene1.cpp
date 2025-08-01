#include "myscene1.h"



#include "../../engine/core/include/app/app.h"
#include "../../engine/core/include/app/scene.h"




MyScene1::MyScene1(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
        {
            .method = engine::RenderMethod::BlinnPhong,
            .shadowIntensity = 2.0f
        })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene1::init()
{
    // cameras
    auto trsCamera1 = engine::Transform{ { 0.0f, 0.0f, 3.0f } };
    auto camera1 = std::make_shared<engine::FlyCamera>(false);
    camera1->zoom = 25.0f;
    camera1->movementSpeed = 10.0f;
    auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
    getEntityManager().addChild(EntityCamera1);


    auto trsLight1 = engine::Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = std::make_shared<engine::SpotLight>(0);
    light1->intensity = 2.0f;
    light1->cutoff = 12.0f;
    light1->outerCutoff = 48.0f;
    light1->target = glm::vec3(0.0f, 0.0f, 0.0f);
    light1->ambientColor = engine::Color(1.0f);
    light1->diffuseColor = engine::Color(1.0f);
    light1->specularColor = engine::Color(10.0f);


    auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
    getEntityManager().addChild(entityLight1);



    std::vector<std::string> faces
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };

    auto zzz{ engine::BlinnPhongMaterial(engine::Color(0.1f), "textures/container2_diffuse.png", "textures/container2_specular.png") };
    zzz.setCubeMapTexs(faces);


    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/uv_mapper.jpg"), engine::UvMapping(6.0f));
    auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(8.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
    getEntityManager().addChild(entityPlane);



    // billboard
    auto myBillboard = std::make_shared<engine::Billboard>();
    myBillboard->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/grass.png"), engine::UvMapping(1.0f));

    auto trsBillboard = engine::Transform(glm::vec3(0.0f, -0.15f, -3.0f), glm::vec3(0.35f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityBillboard = std::make_shared<engine::Entity>("MyBillboard", myBillboard, trsBillboard);
    getEntityManager().addChild(entityBillboard);


    // cube
    auto myCube = std::make_shared<engine::Cube>();
    myCube->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/uv_mapper.jpg"));

    auto trsCube = engine::Transform(glm::vec3(0.0f, -0.35f, 0.0f), glm::vec3(0.15f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCube = std::make_shared<engine::Entity>("MyCube", myCube, trsCube);
    getEntityManager().addChild(entityCube);


    // cylinder
    auto myCylinder = std::make_shared<engine::Cylinder>();
    myCylinder->radius = 0.1f;
    myCylinder->height = 0.3f;
    myCylinder->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/uv_mapper.jpg"), engine::UvMapping(1.0f));

    auto trsCylinder = engine::Transform(glm::vec3(0.5f, -0.35f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCylinder = std::make_shared<engine::Entity>("MyCylinder", myCylinder, trsCylinder);
    getEntityManager().addChild(entityCylinder);


    // cone
    auto myCone = std::make_shared<engine::Cone>();
    myCone->radius = 0.1f;
    myCone->height = 0.3f;
    myCone->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/uv_mapper.jpg"), engine::UvMapping(1.0f));

    auto trsCone = engine::Transform(glm::vec3(1.0f, -0.35f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entityCone = std::make_shared<engine::Entity>("MyCone", myCone, trsCone);
    getEntityManager().addChild(entityCone);


    // sphere
    auto mySphere = std::make_shared<engine::Sphere>();
    mySphere->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f), "textures/uv_mapper.jpg"), engine::UvMapping(1.0f));

    auto trsSphere = engine::Transform(glm::vec3(1.5f, -0.35f, 0.0f), glm::vec3(0.2f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto entitySphere = std::make_shared<engine::Entity>("MySphere", mySphere, trsSphere);
    getEntityManager().addChild(entitySphere);








    // cushion model
    std::shared_ptr<engine::Model> cushionModel = std::make_shared<engine::Model>("models/cushion/cushion.glb");
    auto trsCushion = engine::Transform(glm::vec3(-0.5f, -0.35f, 0.0f), glm::vec3(0.10f), glm::vec3(0.0f, 45.0f, 0.0f));
    auto entityCushion = std::make_shared<engine::Entity>("MyCushion", cushionModel, trsCushion);
    getEntityManager().addChild(entityCushion);


    // backpack model
    std::shared_ptr<engine::Model> backpackModel = std::make_shared<engine::Model>("models/backpack/backpack.glb");
    auto trsBackpack = engine::Transform(glm::vec3(-1.0f, -0.25f, 0.0f), glm::vec3(0.12f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityBackpack = std::make_shared<engine::Entity>("MyBackpack", backpackModel, trsBackpack);
    getEntityManager().addChild(entityBackpack);





    
    // skybox
    auto renderer = dynamic_cast<engine::BlinnPhongRenderer*>(getRenderer());
    if (renderer)
    {
        renderer->setSkybox(faces);
    }



    ourText.setup(app->window, FONT_PATH, 28);
}


void MyScene1::key_callback(int key, int scancode, int action, int mods)
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

void MyScene1::mouse_callback(double xposIn, double yposIn)
{
    engine::Scene::mouse_callback(xposIn, yposIn);

    if (show_window)
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
    engine::Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene1::gamepad_callback(const GLFWgamepadstate& state)
{
    engine::Scene::gamepad_callback(state);
}

void MyScene1::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}


void MyScene1::update(engine::Shader& shader)
{
    UNREFERENCED_PARAMETER(shader);

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        myCushion->transform.setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
    }

    auto myBackpack = getEntityManager().findEntityByName("MyBackpack");
    if (myBackpack)
    {
        myBackpack->transform.setLocalRotation(glm::vec3(90.0f, rotation, 0.0f));
    }

    rotation += deltaTime * 10.0f;
}

void MyScene1::updateUI()
{
    // render HUD / UI
    ourText.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}

void MyScene1::clean()
{
    // clean up any resources
}
