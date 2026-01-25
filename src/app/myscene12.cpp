#include "myscene12.h"

using namespace std;
using namespace glm;
using namespace engine;

MyScene12::MyScene12(string _title, App* _app) : Scene(_title, _app, SceneSettings
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


void MyScene12::init()
{
    // camera
    auto trsCamera1 = Transform{ {0.0f, 1.0f, 5.0f} };
    auto camera1 = make_shared<FlyCamera>(20.0f, -90.0f, -7.0f, 10.0f);
    auto entityCamera1 = make_shared<Entity>("Camera1");
    entityCamera1->addComponent<TransformComponent>(trsCamera1);
    entityCamera1->addComponent<CameraComponent>(camera1);
    getEntityManager().addChild(entityCamera1);
    


    // light
    auto trsLight1 = Transform{ {0.5f, 1.5f, 3.0f} };
    auto light1 = make_shared<SpotLight>();
    light1->intensity = 6.0f;
    light1->cutoff = 12.0f;
    light1->outerCutoff = 48.0f;
    light1->target = vec3(0.0f, 0.0f, 0.0f);
    light1->ambientColor = Color(0.2f);
    light1->diffuseColor = Color(1.0f);
    light1->specularColor = Color(1.0f);
    auto entityLight1 = make_shared<Entity>("Light1");
    entityLight1->addComponent<TransformComponent>(trsLight1);
    entityLight1->addComponent<LightComponent>(light1);
    getEntityManager().addChild(entityLight1);



    // ground
    auto myPlane = make_shared<Plane>();
    myPlane->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/painted-wood/PaintedWood_Color.jpg",
        "textures/pbr/painted-wood/PaintedWood_Normal.jpg",
        "textures/pbr/painted-wood/PaintedWood_Metallic.jpg",
        "textures/pbr/painted-wood/PaintedWood_Roughness.jpg",
        "textures/pbr/painted-wood/PaintedWood_AmbientOcclusion.jpg"), UvMapping(1.0f));
    auto trsPlane = Transform(vec3(0.0f, -0.5f, 0.0f), vec3(3.0f), vec3(0.0f));
    auto entityPlane = make_shared<Entity>("MyPlane");
    entityPlane->addComponent<TransformComponent>(trsPlane);
    entityPlane->addComponent<PrimitiveComponent>(myPlane);
    getEntityManager().addChild(entityPlane);



    // sphere 1
    auto mySphere = make_shared<Sphere>();
    mySphere->setup(make_shared<PBRMaterial>(Color(0.1f),
        "textures/pbr/porcelain/Porcelain_Color.png",
        "textures/pbr/porcelain/Porcelain_Normal.png",
        "textures/pbr/porcelain/Porcelain_Metallic.png",
        "textures/pbr/porcelain/Porcelain_Roughness.png"), UvMapping(1.0f));
    auto trsSphere = Transform(vec3(0.0f, -0.35f, 0.0f), vec3(0.2f));
    auto entitySphere = make_shared<Entity>("MySphere1");
    entitySphere->addComponent<TransformComponent>(trsSphere);
    entitySphere->addComponent<PrimitiveComponent>(mySphere);
    getEntityManager().addChild(entitySphere);



    textFPSCount.setup(app->window, FONT_PATH, 28);
    textPolyCount.setup(app->window, FONT_PATH, 28);
    textMeshCount.setup(app->window, FONT_PATH, 28);
    textPrimitiveCount.setup(app->window, FONT_PATH, 28);

    textDrawnCount.setup(app->window, FONT_PATH, 28);
    textTotalCount.setup(app->window, FONT_PATH, 28);
}


void MyScene12::key_callback(int key, int scancode, int action, int mods)
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

void MyScene12::mouse_callback(double xposIn, double yposIn)
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

void MyScene12::scroll_callback(double xoffset, double yoffset)
{
    Scene::scroll_callback(xoffset, yoffset);

    getActiveCamera()->processMouseScroll(static_cast<float>(yoffset));
}

void MyScene12::gamepad_callback(const GLFWgamepadstate& state)
{
    Scene::gamepad_callback(state);
}

void MyScene12::framebuffer_size_callback(int newWidth, int newHeight)
{
    Scene::framebuffer_size_callback(newWidth, newHeight);
}

void MyScene12::update(Shader& shader)
{
    (void)shader;   //Do nothing
}

void MyScene12::updateUI()
{
    // render HUD / UI
    textFPSCount.draw(format("{:.0f} FPS", framerate), 25.0f, 25.0f, 1.0f, vec3(1.0f));
    textPolyCount.draw(format("{} polys", polycount), app->width - 250.0f, 25.0f, 1.0f, vec3(1.0f));
    textMeshCount.draw(format("{} meshes", meshcount), app->width - 450.0f, 25.0f, 1.0f, vec3(1.0f));
    textPrimitiveCount.draw(format("{} primitives", primitivecount), app->width - 650.0f, 25.0f, 1.0f, vec3(1.0f));

    textDrawnCount.draw(format("{} drawn", inFrustrumCount), 25.0f, 120.0f, 1.0f, vec3(1.0f));
    textTotalCount.draw(format("{} total", totalFrustrumCount), 25.0f, 160.0f, 1.0f, vec3(1.0f));
}

void MyScene12::clean()
{
    // clean up any resources
    textFPSCount.clean();
    textPolyCount.clean();
    textMeshCount.clean();
    textPrimitiveCount.clean();
    textDrawnCount.clean();
    textTotalCount.clean();
}

