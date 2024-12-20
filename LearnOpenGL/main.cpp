#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <iostream>
#include <chrono>
#include <thread>

const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS; // in milliseconds

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
const bool FULLSCREEN = false;



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor.. Later Occulus?..

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

    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(1);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    
    // ..:: Initialization code :: ..
    shader ourShader("shaders/shader.vs", "shaders/shader.fs");

    
    // create a Vertex Buffer Object
    unsigned int VBO = 0;

    // create a Vertex Array Object
    unsigned int VAO = 0;

	// create a Element Buffer Object in order to draw more than a single triangle
    unsigned int EBO = 0;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};


    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };

    //unsigned int indices[] = {  // note that we start from 0!
    //    0, 1, 3,  // first Triangle
    //    1, 2, 3   // second Triangle
    //};


    glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO
	//glGenBuffers(1, &EBO);  // 1 is the uniqueID of the EBO
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);


    // copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copies the previously defined vertex data into the buffer's memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// array of triangles to be able to draw more than a single triangle
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // tell OpenGL how it should interpret the vertex data
    //glVertexAttribPointer(
    //    0, // layout (location = 0)
    //    3, // vec3
    //    GL_FLOAT, // vector of floats
    //    GL_FALSE, // normalized
    //    3 * sizeof(float), // stride
    //    (void*)0 // offset in buffer
    //);
    //glEnableVertexAttribArray(0);

    // position attribute
    glVertexAttribPointer(0, // layout (location = 0)
        3, // vec3
        GL_FLOAT, // vector of floats
        GL_FALSE, // normalized
        6 * sizeof(float), // stride
        (void*)0); // offset in buffer
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, // layout (location = 1)
        3, // vec3
        GL_FLOAT, // vector of floats
        GL_FALSE, // normalized
        6 * sizeof(float), // stride
        (void*)(3 * sizeof(float))); // offset in buffer
    glEnableVertexAttribArray(1);

    


    // unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // unbing vao
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        // input
        processInput(window);

        // render a black background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // clear previous frame rendered
        glClear(GL_COLOR_BUFFER_BIT);

        // be sure to activate the shader
        ourShader.use();

        // update the uniform color
        float timeValue = glfwGetTime();
        float rotatingColor = sin(timeValue);
        ourShader.set4Float("rotatingColor", rotatingColor, rotatingColor, rotatingColor, 1.0f);



        // draw single triagle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        // draw a rectangle made of two triangles
        //glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();

        auto end_time = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY) - (end_time - start_time));
    }

    glBindVertexArray(0);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);
    ourShader.clean();


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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}