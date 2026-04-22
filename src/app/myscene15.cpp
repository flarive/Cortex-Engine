#include "myscene15.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene15::MyScene15(const string& _title, App* _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = true,
        .HDRSkyboxFilePath = "",
        .HDRSkyboxBlurStrength = 0.0f,
        .enableShadows = true,
        .shadowIntensity = 3.0f,
        .shadowMapsTextureSize = 2048
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene15::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 1.0f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(25.0f, -90.0f, 0.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);

    // light
    auto trsLight1 = Transform{ {0.0f, 2.0f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->setIntensity(2.0f);
    light1->setCutoff(10.0f);
    light1->setOuterCutoff(18.0f);
    light1->setTarget(vec3(0.0f, 0.0f, 0.0f));
    light1->setAmbientColor(Color(1.0f));
    light1->setDiffuseColor(Color(1.0f));
    light1->setSpecularColor(Color(1.0f));
    light1->setUseAttenuation(true);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);

    //auto trsLight2 = Transform{ {0.0f, 2.0f, 3.0f} };
    //auto light2 = make_shared<PointLight>();
    //light2->setIntensity(10.0f);
    //light2->setAmbientColor(Color(1.0f));
    //light2->setDiffuseColor(Color(1.0f));
    //light2->setSpecularColor(Color(1.0f));
    //auto entityLight2 = make_shared<Entity>("Light2");
    //entityLight2->addComponent<TransformComponent>(trsLight2);
    //entityLight2->addComponent<LightComponent>(light2);
    //getEntityManager().addChild(entityLight2);

    //auto trsLight1 = Transform{ {0.0f, 2.0f, 3.0f} };
    //auto light1 = make_shared<DirectionalLight>();
    //light1->setIntensity(1.0f);
    //light1->setTarget(vec3(0.0f, 0.0f, 0.0f));
    //light1->setAmbientColor(Color(1.0f));
    //light1->setDiffuseColor(Color(1.0f));
    //light1->setSpecularColor(Color(1.0f));
    //auto entityLight1 = make_shared<Entity>("Light1");
    //entityLight1->addComponent<TransformComponent>(trsLight1);
    //entityLight1->addComponent<LightComponent>(light1);
    //getEntityManager().addChild(entityLight1);



    // ground
    auto myPlane = make_shared<Plane>(true);
    //auto matPlane = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");


    auto matPlane = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/harshbricks/harshbricks-albedo.png",
        "textures/pbr/harshbricks/harshbricks-normal.png",
        "textures/pbr/harshbricks/harshbricks-metalness.png",
        "textures/pbr/harshbricks/harshbricks-roughness.png",
        "textures/pbr/harshbricks/harshbricks-ao.png",
        "textures/pbr/harshbricks/harshbricks-height.png");


    matPlane->useParallaxMapping(true);
    myPlane->setup(matPlane, UvMapping(1.0f));
    auto trsPlane = Transform(vec3(1.5f, -0.5f, -1.5f), vec3(3.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);


    // sphere
    auto mySphere1 = make_shared<Sphere>();
    //auto matSphere1 = make_shared<BlinnPhongMaterial>(Color(0.1f), "textures/bricks2.jpg", "", "textures/bricks2_normal.jpg", "textures/bricks2_disp.jpg");


    auto matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/planks/albedo.jpg",
        "textures/pbr/planks/normal.jpg",
        "textures/pbr/planks/metallic.jpg",
        "textures/pbr/planks/roughness.jpg",
        "textures/pbr/planks/ao.jpg",
        "textures/pbr/planks/displace.jpg");


    matSphere1->useParallaxMapping(true);
    mySphere1->setup(matSphere1, UvMapping(1.0f));
    auto trsSphere1 = Transform(vec3(0.0f, 0.16f, 0.0f), vec3(0.4f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(mySphere1);
    getEntityManager().addChild(entitySphere1);



    textFPSCount.setup(app->window, FONT_PATH, 20);

    textPolyCount.setup(app->window, FONT_PATH, 20);
    textMeshCount.setup(app->window, FONT_PATH, 20);
    textPrimitiveCount.setup(app->window, FONT_PATH, 20);

    textDrawnCount.setup(app->window, FONT_PATH, 20);
    textTotalCount.setup(app->window, FONT_PATH, 20);

    textIncrease.setup(app->window, FONT_PATH, 18);
    textDecrease.setup(app->window, FONT_PATH, 18);

    rect.setup(app->window);
}


void MyScene15::key_callback(int key, int scancode, int action, int mods)
{
    Scene::key_callback(key, scancode, action, mods);

    // Detect Shift key state
    bool shiftPressed = (mods & GLFW_MOD_SHIFT);

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

    if (shiftPressed && key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementParallaxIntensity(-0.01f);
    else if (key == GLFW_KEY_R && (action == GLFW_REPEAT || action == GLFW_PRESS))
        incrementParallaxIntensity(0.01f);
}

void MyScene15::mouse_callback(double xposIn, double yposIn)
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

void MyScene15::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene15::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene15::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene15::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene15::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, Colors::White);

    textPolyCount.draw(format("{} polys", polycount), app->width - 250.0f, 25.0f, 1.0f, Colors::White);
    textMeshCount.draw(format("{} meshes", meshcount), app->width - 450.0f, 25.0f, 1.0f, Colors::White);
    textPrimitiveCount.draw(format("{} primitives", primitivecount), app->width - 650.0f, 25.0f, 1.0f, Colors::White);

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, Colors::White);
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, Colors::White);

    rect.draw(glm::vec2(50, 50), glm::vec2(300, 120), 0.0f, Colors::PapayaWhip, Colors::Yellow, 0.02f);

    textIncrease.draw("R : increase parallax", app->width - 200.0f, app->height - 140.0f, 1.0f, Colors::AliceBlue);
    textDecrease.draw("Shift + R : decrease parallax", app->width - 200.0f, app->height - 160.0f, 1.0f, Colors::AliceBlue);
}

void MyScene15::incrementParallaxIntensity(float intensity)
{
    auto sphere = getEntityManager().findEntityByName("MySphere1");
    if (sphere)
    {
        if (auto component = sphere->getComponent<PrimitiveComponent>())
        {
            if (auto material = component->getPrimitive()->getMaterial())
            {
                float current = material->getParallaxIntensity();
                material->setParallaxIntensity(current + intensity);
            }
        }
    }

    auto plane = getEntityManager().findEntityByName("MyPlane");
    if (plane)
    {
        if (auto component = plane->getComponent<PrimitiveComponent>())
        {
            if (auto material = component->getPrimitive()->getMaterial())
            {
                float current = material->getParallaxIntensity();
                material->setParallaxIntensity(current + intensity);
            }
        }
    }
}

void MyScene15::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();

    rect.clean();
    textIncrease.clean();
    textDecrease.clean();
}
