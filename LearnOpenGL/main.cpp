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
camera cam(glm::vec3(0.0f, 0.0f, 3.0f), true);
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

    


    // Set up vertex data for a 3d cube (position and uvs)
    //const float* vertices = primitive::getCubeVertices();

    //glm::vec3 cubePositions[] =
    //{
    //    glm::vec3(0.0f,  0.0f,  0.0f),
    //    glm::vec3(2.0f,  5.0f, -15.0f),
    //    glm::vec3(-1.5f, -2.2f, -2.5f),
    //    glm::vec3(-3.8f, -2.0f, -12.3f),
    //    glm::vec3(2.4f, -0.4f, -3.5f),
    //    glm::vec3(-1.7f,  3.0f, -7.5f),
    //    glm::vec3(1.3f, -2.0f, -2.5f),
    //    glm::vec3(1.5f,  2.0f, -2.5f),
    //    glm::vec3(1.5f,  0.2f, -1.5f),
    //    glm::vec3(-1.3f,  1.0f, -1.5f)
    //};

    //glm::vec3 pointLightPositions[] =
    //{
    //    glm::vec3(0.7f,  0.2f,  2.0f),
    //    glm::vec3(2.3f, -3.3f, -4.0f),
    //    glm::vec3(-4.0f,  2.0f, -12.0f),
    //    glm::vec3(0.0f,  0.0f, -3.0f)
    //};


    // cubes
    // create a Vertex Array Object
    //unsigned int cubeVAO = 0;

    //glGenVertexArrays(1, &cubeVAO);  // 1 is the uniqueID of the VAO
    //glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

    //

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    //glBindVertexArray(cubeVAO);


    //

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), vertices, GL_STATIC_DRAW);

 


    //GLsizei stride = 8;

    // position attribute (XYZ)
    // layout (location = 0), vec3, vector of floats, normalized, stride, offset in buffer
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0); // stride 0 to 2


    //// normal attribute (XYZ)
    //// layout(location = 1), vec3, vector of floats, normalized, stride, offset in buffer
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1); // stride 5 to 5

    //// texture coord attribute (RGB)
    //// layout(location = 2), vec3, vector of floats, normalized, stride, offset in buffer
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
    //glEnableVertexAttribArray(2); // stride 3 to 4

    //

    //


    //// light
    //// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    //unsigned int lightCubeVAO = 0;
    //glGenVertexArrays(1, &lightCubeVAO);
    //glBindVertexArray(lightCubeVAO);

    //// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);



    //// load texture
    //// -----------------------------------------------
    //unsigned int diffuseMap = texture_helper::soil_load_texture("container2.png", false, true);
    //unsigned int specularMap = texture_helper::soil_load_texture("container2_specular.png", true, false);


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



        

        // activate shader
        lightingShader.use();
        lightingShader.setVec3("viewPos", cam.Position);


        ourLights.Draw(lightCubeShader, lightingShader);



        ///*
        //    Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
        //    the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
        //    by defining light types as classes and set their values in there, or by using a more efficient uniform approach
        //    by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
        //*/
        //// directional light
        ////lightingShader.setBool("dirLight.use", false);
        ////lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ////lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ////lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ////lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        //// point light 1
        //lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        //lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[0].constant", 1.0f);
        //lightingShader.setFloat("pointLights[0].linear", 0.09f);
        //lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        //// point light 2
        //lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        //lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[1].constant", 1.0f);
        //lightingShader.setFloat("pointLights[1].linear", 0.09f);
        //lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        //// point light 3
        //lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        //lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[2].constant", 1.0f);
        //lightingShader.setFloat("pointLights[2].linear", 0.09f);
        //lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        //// point light 4
        //lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        //lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        //lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        //lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        //lightingShader.setFloat("pointLights[3].constant", 1.0f);
        //lightingShader.setFloat("pointLights[3].linear", 0.09f);
        //lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        //// spotLight
        ////lightingShader.setBool("spotLight.use", false);
        ////lightingShader.setVec3("spotLight.position", cam.Position);
        ////lightingShader.setVec3("spotLight.direction", cam.Front);
        ////lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ////lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ////lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ////lightingShader.setFloat("spotLight.constant", 1.0f);
        ////lightingShader.setFloat("spotLight.linear", 0.09f);
        ////lightingShader.setFloat("spotLight.quadratic", 0.032f);
        ////lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ////lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = cam.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        //lightingShader.setMat4("model", model);


        // bind diffuse map
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, diffuseMap);
        //// bind specular map
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, specularMap);


        // render the cubes
        //glBindVertexArray(cubeVAO);
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    // calculate the model matrix for each object and pass it to shader before drawing
        //    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        //    model = glm::translate(model, cubePositions[i]);
        //    float angle = 20.0f * i;
        //    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //    lightingShader.setMat4("model", model);

        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

        // render the loaded model
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        lightingShader.setMat4("model", model);
        ourModel1.Draw(lightingShader);

        // render the loaded model
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0));	// it's a bit too big for our scene, so scale it down
        //lightingShader.setMat4("model", model);
        //ourModel2.Draw(lightingShader);


        // render test cubes
        //ourCubes.Draw(lightingShader);

        

        //// also draw the lamp object(s)
        
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        //// we now draw as many light bulbs as we have point lights.
        //glBindVertexArray(lightCubeVAO);
        //for (unsigned int i = 0; i < 4; i++)
        //{
        //    model = glm::mat4(1.0f);
        //    model = glm::translate(model, pointLightPositions[i]);
        //    model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        //    lightCubeShader.setMat4("model", model);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}



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