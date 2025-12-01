//#include "core/include/app/app.h"
//#include "core/include/app/scene.h"
//#include "core/include/managers/log_manager.h"
//
//#include "app/myapp1.h"
//
//#include "app/myscene1.h" // blinnphong with skybox
//#include "app/myscene2.h" // blinnphong cushion
//#include "app/myscene3.h" // pbr balls with HDR background
//#include "app/myscene4.h" // pbr cushion
//#include "app/myscene5.h" // pbr buddha
//#include "app/myscene6.h" // pbr rotating helmet
//#include "app/myscene7.h" // pbr multiple helmets
//#include "app/myscene8.h" // monochromatic point shadow
//#include "app/myscene9.h" // orbit camera
//#include "app/myscene10.h" // area lights demo
//#include "app/myscene11.h" // blinnphong animated character
//
//using namespace engine;
//
//// make it easier to switch between apps
//using MyApp = MyApp1;
//using MyScene = MyScene11;
//
//App* myApp{};
//Scene* myScene{};
//
//
//// Auto select Nvidia or AMD GPU instead of builtin intel GPU
//extern "C" {
//    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}
//
//
//// Needed by main method
//static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
//static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
//static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
//static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
//static void windowRefreshCallback(GLFWwindow* window);
//static void gamepadUpdate();
//
//
//
//// Startup method
//int main(int, char**)
//{
//    // Init the app
//    myApp = new MyApp("MyApp", 1280, 720, false);
//    if (myApp)
//    {
//        // Init a scene in the app
//        myScene = new MyScene("MyScene", myApp);
//        if (myScene)
//        {
//            myScene->initialize();
//
//            glfwSetFramebufferSizeCallback(myScene->getWindow(), framebufferSizeCallback);
//            glfwSetKeyCallback(myScene->getWindow(), keyCallback);
//            glfwSetCursorPosCallback(myScene->getWindow(), mouseCallback);
//            glfwSetScrollCallback(myScene->getWindow(), scrollCallback);
//            glfwSetWindowRefreshCallback(myScene->getWindow(), windowRefreshCallback);
//
//            int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
//            if (present > 0)
//            {
//                const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
//                logger.info("Joystick present {}", name);
//            }
//
//            // start game loop
//            while (myApp->isRunning())
//            {
//                gamepadUpdate(); // Update gamepad state
//                myScene->gameLoop();
//            }
//
//            myScene->exit();
//            myApp->exit();
//        }
//        else
//        {
//            logger.error("Failed to create the scene");
//            return -1;
//        }
//    }
//    else
//    {
//        logger.error("Failed to create application");
//        return -1;
//    }
//
//    return 0;
//}
//
//// glfw: whenever a keyboard key is pressed, this callback is called
//// -----------------------------------------------------------------
//static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    (void)window;   //Do nothing
//
//    ((MyScene*)myScene)->key_callback(key, scancode, action, mods);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    (void)window;   //Do nothing
//
//    ((MyScene*)myScene)->mouse_callback(xposIn, yposIn);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    (void)window;   //Do nothing
//
//    ((MyScene*)myScene)->scroll_callback(xoffset, yoffset);
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
//{
//    (void)window;   //Do nothing
//
//    ((MyScene*)myScene)->framebuffer_size_callback(width, height);
//}
//
//static void windowRefreshCallback(GLFWwindow* window)
//{
//    (void)window;   //Do nothing
//
//    ((MyScene*)myScene)->window_refresh_callback();
//}
//
//// Poll gamepad input and forward to MyApp
//// ---------------------------------------
//static void gamepadUpdate()
//{
//    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
//    {
//        GLFWgamepadstate state;
//        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
//        {
//            ((MyScene*)myScene)->gamepad_callback(state);
//        }
//    }
//}




#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/include/shader.h"
#include "core/include/cameras/fly_camera.h"
#include "core/include/models/animator.h"
#include "core/include/models/animated_model.h"
#include "core/include/tools/file_system.h"



#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
engine::FlyCamera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    engine::Shader ourShader{};
    ourShader.init("anim_model", "anim_model.vs", "anim_model.fs");


    // load models
    // -----------
    const std::string aaa = engine::FileSystem::getPath("resources/objects/vampire/dancing_vampire.dae");
    engine::AnimatedModel ourModel(engine::FileSystem::getPath("resources/objects/vampire/dancing_vampire.dae"));
    engine::Animation danceAnimation(aaa, &ourModel);
    engine::Animator animator(&danceAnimation);


    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        animator.updateAnimation(deltaTime);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        auto transforms = animator.getFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
            ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(.5f, .5f, .5f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.draw(ourShader);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(engine::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(engine::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(engine::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(engine::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    camera.processMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}
