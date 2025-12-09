#include "myscene3.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene3::MyScene3(string _title, App* _app) : Scene(_title, _app, SceneSettings
    {
        .method = RenderMethod::PBR,
        .HDRSkyboxHide = false,
        .HDRSkyboxFilePath = "textures/hdr/newport_loft.hdr",
        .shadowIntensity = 1.5f,
        .iblDiffuseIntensity = 1.0f,
        .iblSpecularIntensity = 1.0f,
        .enableGammaCorrection = true
    })
{
    // my application specific state gets initialized here

    lastX = app->width / 2.0f;
    lastY = app->height / 2.0f;
}


void MyScene3::init()
{
    // camera
    auto trsCamera1 = Transform{ { 0.0f, -12.0f, 2.0f } };
    auto camera1 = make_shared<FlyCamera>();
    camera1->zoom = 75.0f;
    camera1->movementSpeed = 10.0f;
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);



    // lights
    auto trsLight5 = Transform{ { 0.0f, 4.0f, -2.0f } };
    auto light5 = make_shared<SpotLight>();
    light5->intensity = 10.0f;
    light5->cutoff = 12.5f;
    light5->outerCutoff = 27.5f;
    light5->target = vec3(0.0f, 0.0f, -4.0f);
    auto entityLight5 = make_shared<Entity>("Light5");
    entityLight5->addComponent<TransformComponent>(trsLight5);
    entityLight5->addComponent<LightComponent>(light5);
    getEntityManager().addChild(entityLight5);



    auto trsLight1 = Transform{ { -10.0f, 10.0f, 10.0f } };
    auto light1 = make_shared<PointLight>();
    light1->intensity = 10.0f;
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    auto trsLight2 = Transform{ { 10.0f, 10.0f, 10.0f } };
    auto light2 = make_shared<PointLight>();
    light2->intensity = 10.0f;
    auto entityLight2 = make_shared<Entity>("Light2");
    entityLight2->addComponent<TransformComponent>(trsLight2);
    entityLight2->addComponent<LightComponent>(light2);
    getEntityManager().addChild(entityLight2);



    auto trsLight3 = Transform{ { -10.0f, -10.0f, 10.0f } };
    auto light3 = make_shared<PointLight>();
    light3->intensity = 10.0f;
    auto entityLight3 = make_shared<Entity>("Light3");
    entityLight3->addComponent<TransformComponent>(trsLight3);
    entityLight3->addComponent<LightComponent>(light3);
    getEntityManager().addChild(entityLight3);



    auto trsLight4 = Transform{ { 10.0f, -10.0f, 10.0f } };
    auto light4 = make_shared<PointLight>();
    light4->intensity = 10.0f;
    auto entityLight4 = make_shared<Entity>("Light4");
    entityLight4->addComponent<TransformComponent>(trsLight4);
    entityLight4->addComponent<LightComponent>(light4);
    getEntityManager().addChild(entityLight4);




    // ground
    auto myPlane = make_shared<Plane>();
    auto matPlane = make_shared<PBRMaterial>(Color(1.0f),
        "models/sphere/cliff/albedo.jpg",
        "models/sphere/cliff/normal.jpg",
        "models/sphere/cliff/metallic.jpg",
        "models/sphere/cliff/roughness.jpg",
        "models/sphere/cliff/ao.jpg",
        "models/sphere/cliff/height.jpg");
    matPlane->setNormalIntensity(1.0f);
    myPlane->setup(matPlane, UvMapping(2.0f));
    auto trsPlane = Transform(vec3(0.0f, -15.0f, -15.0f), vec3(12.0f), vec3(0.0f, 0.0f, 0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);



    // sphere models
    auto redSciFiMetalSphere = make_shared<Sphere>();

    auto matSphere1 = make_shared<PBRMaterial>(Color(0.1f),
        "models/sphere/rounded-metal-cubes/albedo.dds",
        "models/sphere/rounded-metal-cubes/normal.png",
        "models/sphere/rounded-metal-cubes/metallic.png",
        "models/sphere/rounded-metal-cubes/roughness.png",
        "models/sphere/rounded-metal-cubes/ao.png",
        "models/sphere/rounded-metal-cubes/height.png");
    matSphere1->setNormalIntensity(5.0f);

    redSciFiMetalSphere->setup(matSphere1, UvMapping(2.0f));

    auto trsSphere1 = Transform(vec3(-5.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere1 = make_shared<Entity>("MySphere1");
    entitySphere1->addComponent<TransformComponent>(trsSphere1);
    entitySphere1->addComponent<PrimitiveComponent>(redSciFiMetalSphere);
    getEntityManager().addChild(entitySphere1);




    auto rustedIronSphere = make_shared<Sphere>();
    rustedIronSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/rusted_iron/albedo.png",
        "textures/pbr/rusted_iron/normal.png",
        "textures/pbr/rusted_iron/metallic.png",
        "textures/pbr/rusted_iron/roughness.png",
        "textures/pbr/rusted_iron/ao.png",
        "textures/pbr/rusted_iron/height.png"));

    auto trsSphere2 = Transform(vec3(-3.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere2 = make_shared<Entity>("MySphere2");
    entitySphere2->addComponent<TransformComponent>(trsSphere2);
    entitySphere2->addComponent<PrimitiveComponent>(rustedIronSphere);
    getEntityManager().addChild(entitySphere2);



    auto goldSphere = make_shared<Sphere>();
    goldSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/gold/albedo.png",
        "textures/pbr/gold/normal.png",
        "textures/pbr/gold/metallic.png",
        "textures/pbr/gold/roughness.png",
        "textures/pbr/gold/ao.png"));

    auto trsSphere3 = Transform(vec3(-1.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere3 = make_shared<Entity>("MySphere3");
    entitySphere3->addComponent<TransformComponent>(trsSphere3);
    entitySphere3->addComponent<PrimitiveComponent>(goldSphere);
    getEntityManager().addChild(entitySphere3);



    auto grassSphere = make_shared<Sphere>();
    grassSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/grass/albedo.png",
        "textures/pbr/grass/normal.png",
        "textures/pbr/grass/metallic.png",
        "textures/pbr/grass/roughness.png",
        "textures/pbr/grass/ao.png"));

    auto trsSphere4 = Transform(vec3(1.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere4 = make_shared<Entity>("MySphere4");
    entitySphere4->addComponent<TransformComponent>(trsSphere4);
    entitySphere4->addComponent<PrimitiveComponent>(grassSphere);
    getEntityManager().addChild(entitySphere4);



    auto plasticSphere = make_shared<Sphere>();
    plasticSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/plastic/albedo.png",
        "textures/pbr/plastic/normal.png",
        "textures/pbr/plastic/metallic.png",
        "textures/pbr/plastic/roughness.png",
        "textures/pbr/plastic/ao.png", "", 1.0f));

    auto trsSphere5 = Transform(vec3(3.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere5 = make_shared<Entity>("MySphere5");
    entitySphere5->addComponent<TransformComponent>(trsSphere5);
    entitySphere5->addComponent<PrimitiveComponent>(plasticSphere);
    getEntityManager().addChild(entitySphere5);



    auto wallSphere = make_shared<Sphere>();
    wallSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/wall/albedo.png",
        "textures/pbr/wall/normal.png",
        "textures/pbr/wall/metallic.png",
        "textures/pbr/wall/roughness.png",
        "textures/pbr/wall/ao.png"), UvMapping(1.0f));

    auto trsSphere6 = Transform(vec3(5.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere6 = make_shared<Entity>("MySphere6");
    entitySphere6->addComponent<TransformComponent>(trsSphere6);
    entitySphere6->addComponent<PrimitiveComponent>(wallSphere);
    getEntityManager().addChild(entitySphere6);


    //auto wallPlane = wallSphere.getMaterial();
    //if (wallPlane)
    //{
    //    wallPlane->setNormalIntensity(1.0f);
    //}



    auto bronzeSphere = make_shared<Sphere>();
    bronzeSphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/bronze/albedo.png",
        "textures/pbr/bronze/normal.png",
        "textures/pbr/bronze/metallic.png",
        "textures/pbr/bronze/roughness.png",
        "textures/pbr/bronze/ao.png"), UvMapping(1.0f));

    auto trsSphere7 = Transform(vec3(7.0f, -14.0f, -10.0f), vec3(1.0f));
    auto entitySphere7 = make_shared<Entity>("MySphere6");
    entitySphere7->addComponent<TransformComponent>(trsSphere7);
    entitySphere7->addComponent<PrimitiveComponent>(bronzeSphere);
    getEntityManager().addChild(entitySphere7);

    //auto bronzeMat = bronzeSphere.getMaterial();
    //if (bronzeMat)
    //{
    //    bronzeMat->setAmbientIntensity(5.0f);
    //}


    ourText.setup(app->window, FONT_PATH, 28);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
void MyScene3::key_callback(int key, int scancode, int action, int mods)
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


void MyScene3::mouse_callback(double xposIn, double yposIn)
{
    //(void)xposIn;   //Do nothing
    //(void)yposIn;   //Do nothing

    Scene::mouse_callback(xposIn, yposIn);

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

void MyScene3::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene3::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);


}

void MyScene3::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);

    ourText.setup(app->window, FONT_PATH, 28);
}

void MyScene3::update(Shader& shader)
{
    // draw scene and UI in framebuffer
    drawScene(shader);
}

void MyScene3::updateUI()
{
    drawUI();
}

void MyScene3::clean()
{
    // clean up any resources
    //rustedIronSphere.clean();
    //goldSphere.clean();
    //grassSphere.clean();
    //plasticSphere.clean();
    //wallSphere.clean();
    //bronzeSphere.clean();
    //ourPlane.clean();
}

void MyScene3::drawScene(Shader& shader)
{
    (void)shader;   //Do nothing

    rotation += deltaTime * 10.0f;
}

void MyScene3::drawUI()
{
    // render HUD / UI
    ourText.draw(format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, vec3(0.5, 0.8f, 0.2f));
}
