#include "core/include/app/app.h"
#include "core/include/app/scene.h"
#include "core/include/managers/log_manager.h"

#include "app/myapp1.h"

#include "app/myscene1.h" // blinnphong with skybox
#include "app/myscene2.h" // blinnphong cushion
#include "app/myscene3.h" // PBR balls with HDR background
#include "app/myscene4.h" // PBR cushion
#include "app/myscene5.h" // PBR buddha
#include "app/myscene6.h" // PBR rotating helmet
#include "app/myscene7.h" // PBR multiple helmets
#include "app/myscene8.h" // blinnphong monochromatic point shadow
#include "app/myscene9.h" // PBR orbit camera
#include "app/myscene10.h" // PBR area lights demo
#include "app/myscene11.h" // blinnphong animated character
#include "app/myscene12.h" // PBR wood plane with misc materials
#include "app/myscene13.h" // blinnphong particles
#include "app/myscene14.h" // blinnphong terrain

using namespace engine;

// make it easier to switch between apps
using MyApp = MyApp1;
using MyScene = MyScene14;

App* myApp{};
Scene* myScene{};


// Auto select Nvidia or AMD GPU instead of builtin intel GPU
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


// Needed by main method
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void windowRefreshCallback(GLFWwindow* window);
static void gamepadUpdate();



// Startup method
int main(int, char**)
{
    // Init the app
    myApp = new MyApp("MyApp", 1280, 720, false);
    //myApp = new MyApp("MyApp", 320, 240, false);
    if (myApp)
    {
        // Init a scene in the app
        myScene = new MyScene("MyScene", myApp);
        if (myScene)
        {
            myScene->initialize();

            glfwSetFramebufferSizeCallback(myScene->getWindow(), framebufferSizeCallback);
            glfwSetKeyCallback(myScene->getWindow(), keyCallback);
            glfwSetCursorPosCallback(myScene->getWindow(), mouseCallback);
            glfwSetScrollCallback(myScene->getWindow(), scrollCallback);
            glfwSetWindowRefreshCallback(myScene->getWindow(), windowRefreshCallback);

            int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
            if (present > 0)
            {
                const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
                logger.info("Joystick present {}", name);
            }

            // start game loop
            while (myApp->isRunning())
            {
                gamepadUpdate(); // Update gamepad state
                myScene->gameLoop();
            }

            myScene->exit();
            myApp->exit();
        }
        else
        {
            logger.error("Failed to create the scene");
            return -1;
        }
    }
    else
    {
        logger.error("Failed to create application");
        return -1;
    }

    return 0;
}

// glfw: whenever a keyboard key is pressed, this callback is called
// -----------------------------------------------------------------
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->key_callback(key, scancode, action, mods);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->mouse_callback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->framebuffer_size_callback(width, height);
}

static void windowRefreshCallback(GLFWwindow* window)
{
    (void)window;   //Do nothing

    ((MyScene*)myScene)->window_refresh_callback();
}

// Poll gamepad input and forward to MyApp
// ---------------------------------------
static void gamepadUpdate()
{
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            ((MyScene*)myScene)->gamepad_callback(state);
        }
    }
}






//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <iostream>
//#include <vector>
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//
//// settings
//const unsigned int SCR_WIDTH = 1280;
//const unsigned int SCR_HEIGHT = 720;
//const unsigned int NUM_PATCH_PTS = 4;
//
//// camera - give pretty starting point
//FlyCamera camera(10.0f, -128.1f, -42.4f, 10.0f, 1.0f, glm::vec3(67.0f, 627.5f, 169.9f));
//
//
//
//
//
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//int main()
//{
//    // glfw: initialize and configure
//    // ------------------------------
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//    // glfw window creation
//    // --------------------
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL: Terrain GPU", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetKeyCallback(window, key_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    // glad: load all OpenGL function pointers
//    // ---------------------------------------
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    GLint maxTessLevel;
//    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
//    std::cout << "Max available tess level: " << maxTessLevel << std::endl;
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);
//
//    // build and compile our shader program
//    // ------------------------------------
//    Shader tessHeightMapShader{};
//    tessHeightMapShader.init("height", "shaders/height.vert", "shaders/height.frag", nullptr, "shaders/height.tcs", "shaders/height.tes");
//
//
//
//
//    // load and create a texture
//    // -------------------------
//    engine::TextureData data = Texture::loadTextureExtended("textures/height/iceland_heightmap.png", true, false);
//    unsigned int texture = std::get<0>(data);
//    int width = std::get<2>(data);
//    int height = std::get<3>(data);
//
//    if (texture > 0)
//    {
//        tessHeightMapShader.setInt("heightMap", 0);
//        std::cout << "Loaded heightmap of size " << width << " x " << height << std::endl;
//    }
//    else
//    {
//        std::cout << "Failed to load texture" << std::endl;
//    }
//
//
//    // set up vertex data (and buffer(s)) and configure vertex attributes
//    // ------------------------------------------------------------------
//    std::vector<float> vertices;
//
//    unsigned rez = 20;
//    for (unsigned i = 0; i <= rez - 1; i++)
//    {
//        for (unsigned j = 0; j <= rez - 1; j++)
//        {
//            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
//            vertices.push_back(0.0f); // v.y
//            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
//            vertices.push_back(i / (float)rez); // u
//            vertices.push_back(j / (float)rez); // v
//
//            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
//            vertices.push_back(0.0f); // v.y
//            vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
//            vertices.push_back((i + 1) / (float)rez); // u
//            vertices.push_back(j / (float)rez); // v
//
//            vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
//            vertices.push_back(0.0f); // v.y
//            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
//            vertices.push_back(i / (float)rez); // u
//            vertices.push_back((j + 1) / (float)rez); // v
//
//            vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
//            vertices.push_back(0.0f); // v.y
//            vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
//            vertices.push_back((i + 1) / (float)rez); // u
//            vertices.push_back((j + 1) / (float)rez); // v
//        }
//    }
//    std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
//    std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;
//
//    // first, configure the cube's VAO (and terrainVBO)
//    unsigned int terrainVAO, terrainVBO;
//    glGenVertexArrays(1, &terrainVAO);
//    glBindVertexArray(terrainVAO);
//
//    glGenBuffers(1, &terrainVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
//
//    // position attribute
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    // texCoord attribute
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
//    glEnableVertexAttribArray(1);
//
//    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
//
//    // render loop
//    // -----------
//    while (!glfwWindowShouldClose(window))
//    {
//        // per-frame time logic
//        // --------------------
//        float currentFrame = glfwGetTime();
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        // -----
//        processInput(window);
//
//        // render
//        // ------
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // be sure to activate shader when setting uniforms/drawing objects
//        tessHeightMapShader.use();
//
//        // view/projection transformations
//        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
//        glm::mat4 view = camera.getViewMatrix();
//        tessHeightMapShader.setMat4("projection", projection);
//        tessHeightMapShader.setMat4("view", view);
//
//        // world transformation
//        glm::mat4 model = glm::mat4(1.0f);
//        tessHeightMapShader.setMat4("model", model);
//
//        // render the terrain
//        glBindVertexArray(terrainVAO);
//        glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        // -------------------------------------------------------------------------------
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // optional: de-allocate all resources once they've outlived their purpose:
//    // ------------------------------------------------------------------------
//    glDeleteVertexArrays(1, &terrainVAO);
//    glDeleteBuffers(1, &terrainVBO);
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    // ------------------------------------------------------------------
//    glfwTerminate();
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.processKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.processKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.processKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.processKeyboard(RIGHT, deltaTime);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//}
//
//// glfw: whenever a key event occurs, this callback is called
//// ---------------------------------------------------------------------------------------------
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
//{
//    if (action == GLFW_PRESS)
//    {
//        switch (key)
//        {
//        default:
//            break;
//        }
//    }
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.processMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.processMouseScroll(yoffset);
//}


//
//
//
// BlinnPhong.tcs
//#version 400 core
//
//layout(vertices = 3) out; // Example: 3 control points per patch
//
//in gl_PerVertex{
//    vec3 worldPos;
//    vec3 worldNormal;
//    vec2 texCoord;
//} gl_in[]; // Input from vertex shader
//
//out TCS_OUT{
//    vec3 worldPos;
//    vec3 worldNormal;
//    vec2 texCoord;
//} tcs_out[]; // Output to TES
//
//uniform float tessLevelInner = 4.0;
//uniform float tessLevelOuter = 4.0;
//
//void main() {
//    if (gl_InvocationID == 0) {
//        gl_TessLevelInner[0] = tessLevelInner;
//        gl_TessLevelOuter[0] = tessLevelOuter;
//        gl_TessLevelOuter[1] = tessLevelOuter;
//        gl_TessLevelOuter[2] = tessLevelOuter;
//    }
//
//    // Pass through data
//    tcs_out[gl_InvocationID].worldPos = gl_in[gl_InvocationID].worldPos;
//    tcs_out[gl_InvocationID].worldNormal = gl_in[gl_InvocationID].worldNormal;
//    tcs_out[gl_InvocationID].texCoord = gl_in[gl_InvocationID].texCoord;
//
//    // Required for TCS
//    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
//}
//
//
//
//// BlinnPhong.tes
//#version 400 core
//
//layout(triangles, equal_spacing, cw) in;
//
//in TCS_OUT{
//    vec3 worldPos;
//    vec3 worldNormal;
//    vec2 texCoord;
//} tes_in[]; // Input from TCS
//
//out VS_OUT{
//    vec3 FragPos;
//    vec3 Normal;
//    vec2 TexCoords;
//    vec3 Tangent;
//    vec3 Bitangent;
//    vec4 FragPosLightSpace;
//    vec3 TangentLightPos;
//    vec3 TangentViewPos;
//    vec3 TangentFragPos;
//} vs_out; // Output to fragment shader
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform mat4 lightSpaceMatrix;
//uniform vec3 lightPos;
//uniform vec3 viewPos;
//
//void main() {
//    // Interpolate control points
//    vec3 p0 = tes_in[0].worldPos;
//    vec3 p1 = tes_in[1].worldPos;
//    vec3 p2 = tes_in[2].worldPos;
//    vec3 worldPos = mix(mix(p0, p1, gl_TessCoord.x), p2, gl_TessCoord.y);
//
//    // Interpolate normals and UVs
//    vec3 worldNormal = mix(mix(tes_in[0].worldNormal, tes_in[1].worldNormal, gl_TessCoord.x),
//        tes_in[2].worldNormal, gl_TessCoord.y);
//    vec2 texCoord = mix(mix(tes_in[0].texCoord, tes_in[1].texCoord, gl_TessCoord.x),
//        tes_in[2].texCoord, gl_TessCoord.y);
//
//    // Compute TBN matrix (simplified)
//    vec3 T = normalize(gl_TessCoord.x * tes_in[1].worldPos - tes_in[0].worldPos);
//    vec3 B = normalize(gl_TessCoord.y * tes_in[2].worldPos - tes_in[0].worldPos);
//    vec3 N = normalize(worldNormal);
//    mat3 TBN = transpose(mat3(T, B, N));
//
//    // Output to fragment shader
//    vs_out.FragPos = worldPos;
//    vs_out.Normal = worldNormal;
//    vs_out.TexCoords = texCoord;
//    vs_out.Tangent = T;
//    vs_out.Bitangent = B;
//    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
//    vs_out.TangentLightPos = TBN * lightPos;
//    vs_out.TangentViewPos = TBN * viewPos;
//    vs_out.TangentFragPos = TBN * worldPos;
//
//    // Final clip-space position
//    gl_Position = projection * view * vec4(worldPos, 1.0);
//}
//
//
//// VertexShader.vert
//out VS_OUT{
//    vec3 FragPos;
//    vec3 Normal;
//    vec2 TexCoords;
//    vec3 Tangent;
//    vec3 Bitangent;
//    vec4 FragPosLightSpace;
//    vec3 TangentLightPos;
//    vec3 TangentViewPos;
//    vec3 TangentFragPos;
//} vs_out;
//
//
//8. Summary of Fixes
//
//Vertex Shader :
//
//Output gl_out[] for tessellation.
//Output vs_out for non - tessellated rendering.
//
//TCS :
//
//    Input : gl_in[](from vertex shader).
//    Output : tcs_out[](for TES).
//
//    TES :
//
//    Input : tes_in[](from TCS).
//    Output : vs_out(for fragment shader).
//
//    Fragment Shader :
//
//Input: fs_in(from TES or vertex shader).
//
//
