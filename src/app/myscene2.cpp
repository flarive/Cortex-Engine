#include "myscene2.h"

#include "../../engine/core/include/app/app.h"
#include "../../engine/core/include/app/scene.h"

MyScene2::MyScene2(std::string _title, engine::App* _app) : engine::Scene(_title, _app, engine::SceneSettings
        {
            .method = engine::RenderMethod::BlinnPhong,
            .shadowIntensity = 1.0f
        })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene2::init()
{
    // cameras
    auto trsCamera1 = engine::Transform{ { 0.0f, 0.0f, 3.0f } };
    auto camera1 = std::make_shared<engine::LegacyCamera>(false);
    camera1->zoom = 25.0f;
    camera1->movementSpeed = 10.0f;
    auto EntityCamera1 = std::make_shared<engine::Entity>("Camera1", camera1, trsCamera1);
    getEntityManager().addChild(EntityCamera1);


    // lights
    auto trsLight1 = engine::Transform{ {0.0f, 1.0f, 3.0f} };
    auto light1 = std::make_shared<engine::SpotLight>(0);
    light1->intensity = 2.5f;
    light1->cutoff = 12.0f;
    light1->outerCutoff = 18.0f;
    light1->target = glm::vec3(0.0f, 0.0f, 0.0f);
    light1->ambientColor = engine::Color(1.0f);
    light1->diffuseColor = engine::Color(1.0f);
    light1->specularColor = engine::Color(10.0f);

    auto entityLight1 = std::make_shared<engine::Entity>("Light1", light1, trsLight1);
    getEntityManager().addChild(entityLight1);



    // ground
    auto myPlane = std::make_shared<engine::Plane>();
    myPlane->setup(std::make_shared<engine::BlinnPhongMaterial>(engine::Color(0.1f),
        "textures/wood_diffuse.png",
        "textures/wood_specular.png"), engine::UvMapping(2.0f));

    auto trsPlane = engine::Transform(glm::vec3(0.0f, -0.5f, -6.0f), glm::vec3(10.0f), glm::vec3(90.0f, 0.0f, 0.0f));
    auto entityPlane = std::make_shared<engine::Entity>("MyPlane", myPlane, trsPlane);
    getEntityManager().addChild(entityPlane);




    // cushion model
    auto cushionModel = std::make_shared<engine::Model>("models/cushion/cushion.glb", true);
    auto trsCushion = engine::Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f), glm::vec3(0.0f, 45.0f, 0.0f));
    auto entityCushion = std::make_shared<engine::Entity>("MyCushion", cushionModel, trsCushion);
    getEntityManager().addChild(entityCushion);




    textFPSCount.setup(app->window, FONT_PATH, 28);
    textPolyCount.setup(app->window, FONT_PATH, 28);
    textMeshCount.setup(app->window, FONT_PATH, 28);
    textPrimitiveCount.setup(app->window, FONT_PATH, 28);
}


void MyScene2::key_callback(int key, int scancode, int action, int mods)
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

void MyScene2::mouse_callback(double xposIn, double yposIn)
{
    UNREFERENCED_PARAMETER(xposIn);
    UNREFERENCED_PARAMETER(yposIn);

    //engine::Scene::mouse_callback(xposIn, yposIn);

    //if (show_window)
    //    return;

    //float xpos{ static_cast<float>(xposIn) };
    //float ypos{ static_cast<float>(yposIn) };

    //if (firstMouse)
    //{
    //    lastX = xpos;
    //    lastY = ypos;
    //    firstMouse = false;
    //}

    //float xoffset{ xpos - lastX };
    //float yoffset{ lastY - ypos }; // reversed since y-coordinates go from bottom to top

    //lastX = xpos;
    //lastY = ypos;

    //getActiveCamera()->processMouseMovement(xoffset, yoffset);
}

void MyScene2::scroll_callback(double xoffset, double yoffset)
{
    UNREFERENCED_PARAMETER(xoffset);
    UNREFERENCED_PARAMETER(yoffset);


    //engine::Scene::scroll_callback(xoffset, yoffset);

    //getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene2::gamepad_callback(const GLFWgamepadstate& state)
{
    UNREFERENCED_PARAMETER(state);
}

void MyScene2::framebuffer_size_callback(int newWidth, int newHeight)
{
    engine::Scene::framebuffer_size_callback(newWidth, newHeight);
}


void MyScene2::update(engine::Shader& shader)
{
    UNREFERENCED_PARAMETER(shader);

    auto myCushion = getEntityManager().findEntityByName("MyCushion");
    if (myCushion)
    {
        myCushion->transform.setLocalRotation(glm::vec3(0.0f, rotation, 0.0f));
    }

    rotation += deltaTime * 10.0f;
}

void MyScene2::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    textPolyCount.draw(std::format("{} polys", (int)polycount), app->width - 250.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    textMeshCount.draw(std::format("{} meshes", (int)meshcount), app->width - 450.0f, 25.0f, 1.0f, glm::vec3(1.0f));
    textPrimitiveCount.draw(std::format("{} primitives", (int)primitivecount), app->width - 650.0f, 25.0f, 1.0f, glm::vec3(1.0f));
}

void MyScene2::clean()
{
    // clean up any resources
    //ourCube1.clean();
    ////ourSphere1.clean();
    //ourPlane.clean();
}
