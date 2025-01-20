#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "texture_helper.h"
#include "primitive.h"

#include "cubes.h"
#include "lights.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS; // in milliseconds

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const bool FULLSCREEN = false;

// camera
Camera cam(glm::vec3(0.0f, 0.0f, 3.0f), true);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor

    const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);
    if (FULLSCREEN)
    {
        SCR_WIDTH = mode->width;
        SCR_HEIGHT = mode->height;
    }


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", FULLSCREEN ? MyMonitor : NULL, nullptr);
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

    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(1);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    

    // ..:: Initialization code :: ..


    
    // enable z buffer (depth test)
    glEnable(GL_DEPTH_TEST);

    // load shaders
    Shader lightingShader("shaders/shader.vertex", "shaders/shader.frag"); // scene textured cube shader
    Shader lightCubeShader("shaders/light_cube.vertex", "shaders/light_cube.frag"); // light cube shader

    // load models
    Model ourModel1("models/backpack/backpack.obj");
    Model ourModel2("models/cushion/cushion.obj");

    // temp test
    Cubes ourCubes(9);

    // lights
    Lights ourLights;



    
    // create a Vertex Buffer Object
    unsigned int VBO = 0;

    

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    lightingShader.use();
    //lightingShader.setInt("texture1", 0);
    //lightingShader.setInt("texture2", 1);

    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    //lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    //lightingShader.setInt("material.diffuse", 0); // texture 0
    //lightingShader.setInt("material.specular", 1); // texture 1
    //lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 32.0f);




    // unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //// unbing vao
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL


     // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
    // -----------------------------------------------------------------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    lightingShader.setMat4("projection", projection);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // ..:: Render loop code :: ..

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        auto start_time = std::chrono::high_resolution_clock::now();

        // input
        processInput(window);

        // render a black background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // clear previous frame rendered
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        

        
        // lights
        ourLights.Draw(lightCubeShader, lightingShader, projection, view);


        // activate shader
        lightingShader.use();
        lightingShader.setVec3("viewPos", cam.Position);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);




        // render the loaded model
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //lightingShader.setMat4("model", model);
        //ourModel1.Draw(lightingShader);

        // render the loaded model
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0));	// it's a bit too big for our scene, so scale it down
        lightingShader.setMat4("model", model);
        ourModel2.Draw(lightingShader);


        // render test cubes
        //ourCubes.Draw(lightingShader);

        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto end_time = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY) - (end_time - start_time));
    }

    glBindVertexArray(0);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &cubeVAO);
    //glDeleteVertexArrays(1, &lightCubeVAO);
    //glDeleteBuffers(1, &VBO);

    ourLights.clean();

    lightingShader.clean();
	lightCubeShader.clean();


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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.ProcessKeyboard(DOWN, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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

    cam.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}