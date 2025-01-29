#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "themes/imgui_spectrum.h"

#include "app/common_defines.h"

#include "app/shader.h"
#include "app/camera.h"
#include "app/model.h"
#include "app/cubes.h"
#include "app/lights.h"
#include "app/lights/point_light.h"
#include "app/lights/directional_light.h"
#include "app/lights/spot_light.h"
#include "app/ui/text.h"

#include "app/primitives/cube.h"
#include "app/primitives/plane.h"
#include "app/primitives/billboard.h"
#include "app/primitives/skybox.h"

#include "app/texture_helper.h"

#include <iostream>
#include <chrono>
#include <thread>

#pragma warning(disable: 4100) // warning C4189: 'yoffset': variable locale initialisée mais non référencée




const int TARGET_FPS = 60;
const int FRAME_DELAY = 1000 / TARGET_FPS; // in milliseconds

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void renderUIWindow(bool show, float framerate);
void drawScene(Shader& lightingShader);
void drawUI(Shader& lightingShader, float framerate);
void cleanScene();




bool show_window = false;
bool key_w_pressed = false;

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


PointLight myPointLight(0);
DirectionalLight myDirectionalLight1(0);
DirectionalLight myDirectionalLight2(1);
SpotLight mySpotLight(0);

//Cubes ourCubes;
Cube ourCube;
Plane ourPlane;
Billboard ourBillboard;

Text ourText;

Skybox ourSkybox;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


    GLFWmonitor* MyMonitor = glfwGetPrimaryMonitor(); // The primary monitor

    const GLFWvidmode* mode = glfwGetVideoMode(MyMonitor);
    if (FULLSCREEN)
    {
        SCR_WIDTH = mode->width;
        SCR_HEIGHT = mode->height;
    }

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", FULLSCREEN ? MyMonitor : NULL, nullptr);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(1);


    
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);




    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }





    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags)
    {
        style.WindowRounding = 0.0f;
        style.ChildRounding = 5.0f;
        style.TabRounding = 5.f;
        style.FrameRounding = 5.f;
        style.GrabRounding = 5.f;
        style.PopupRounding = 5.f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        style.ItemSpacing.y = 8.0; // vertical padding between widgets
        style.FramePadding.x = 8.0; // better widget horizontal padding
        style.FramePadding.y = 4.0; // better widget vertical padding
    }



    // Apply Adobe Spectrum theme
    //https://github.com/adobe/imgui/blob/master/docs/Spectrum.md#imgui-spectrum
    ImGui::Spectrum::StyleColorsSpectrum();
    ImGui::Spectrum::LoadFont(17.0);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    
    // configure global opengl state
    // -----------------------------
    // enable z buffer (depth test) to have correct objects depth ordering
    glEnable(GL_DEPTH_TEST);
    // optim : do not display hidden faces
    // consistent winding orders needed (counter-clockwise by default)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    // anti aliasing MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    // load shaders
    Shader lightingShader("shaders/shader.vertex", "shaders/shader.frag"); // phong illimuniation model and lightning shader
    //Shader depthBufferShader("shaders/depthbuffer.vertex", "shaders/depthbuffer.frag"); // depth buffer debugging shader
    Shader screenShader("shaders/framebuffers_screen.vertex", "shaders/framebuffers_screen.frag"); // framebuffer to screen shader



    
    // screen quad VAO
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), &screenQuadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



    // load models
    Model backpackModel("models/backpack/backpack.obj");
    Model cushionModel("models/cushion/cushion.obj");


    myPointLight.setup();
    myDirectionalLight1.setup();
    myDirectionalLight2.setup();
    mySpotLight.setup();
    
    ourCube.setup();
    ourPlane.setup();
    ourBillboard.setup();

    ourText.setup();

    ourSkybox.setup();

    


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    lightingShader.use();
    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setFloat("material.shininess", 32.0f);



    // shader configuration
    // --------------------
    screenShader.use();
    screenShader.setInt("screenTexture", 0);



    // framebuffer configuration
    // -------------------------
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_window)
            renderUIWindow(show_window, io.Framerate);



        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        auto start_time = std::chrono::high_resolution_clock::now();

        // input
        processInput(window);



        
        // prepare the framebuffer
        // bind to framebuffer and draw scene as we normally would to color texture 
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

        // make sure we clear the framebuffer's content
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        // draw scene and UI in framebuffer
        drawScene(lightingShader);
        drawUI(lightingShader, io.Framerate);






        // draw framebuffer to screen
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // set clear color (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);






        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        auto end_time = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY) - (end_time - start_time));
    }



    glBindVertexArray(0);


    // optional: de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);

    lightingShader.clean();
    screenShader.clean();

    cleanScene();


    // imGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);



    // Detect Shift key state
    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;


    if (shiftPressed && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cam.ProcessKeyboard(YAW_DOWN, deltaTime); // Modify behavior as needed
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);


    if (shiftPressed && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cam.ProcessKeyboard(YAW_UP, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);


    if (shiftPressed && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cam.ProcessKeyboard(PITCH_UP, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, deltaTime);


    if (shiftPressed && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cam.ProcessKeyboard(PITCH_DOWN, deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cam.ProcessKeyboard(DOWN, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (!key_w_pressed) // Only toggle when the key is first pressed
        {
            show_window = !show_window;
            key_w_pressed = true; // Mark the key as pressed
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
    {
        key_w_pressed = false; // Reset the state when the key is released
    }
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

void renderUIWindow(bool show, float framerate)
{
    ImGui::SetNextWindowSize(ImVec2(480, 60), ImGuiCond_Always);

    ImGui::Begin("Hello, world!", &show);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
}

void drawScene(Shader& lightingShader)
{
    lightingShader.use();

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(cam.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = cam.GetViewMatrix();


    // setup lights
    //ourLights.Draw(lightingShader, projection, view);
    myPointLight.draw(lightingShader, projection, view, 1.0f, glm::vec3(0.0f, 0.3f, 2.0f));
    myDirectionalLight1.draw(lightingShader, projection, view, 1.0f, glm::vec3(2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    myDirectionalLight2.draw(lightingShader, projection, view, 0.2f, glm::vec3(-2.0f, 0.3f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    //mySpotLight.draw(lightingShader, projection, view, 1.0f, cam.Position, cam.Front);
    //mySpotLight.draw(lightingShader, projection, view, 1.0f, glm::vec3(0.0f, 0.5f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));



    // activate shader rendering shader
    lightingShader.use();
    lightingShader.setVec3("viewPos", cam.Position);
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);





    // render the loaded model
    //glm::mat4 model1 = glm::mat4(1.0f);
    //model1 = glm::translate(model1, glm::vec3(0.0f, -0.2f, 0.0f)); // translate it down so it's at the center of the scene
    //model1 = glm::scale(model1, glm::vec3(0.3f));	// it's a bit too big for our scene, so scale it down
    //model1 = glm::rotate(model1, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //lightingShader.setMat4("model", model1);
    //cushionModel.Draw(lightingShader);


    // render the loaded model
    //glm::mat4 model2 = glm::mat4(1.0f);
    //model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    //model2 = glm::scale(model2, glm::vec3(0.5f));	// it's a bit too big for our scene, so scale it down
    ////model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //lightingShader.setMat4("model", model2);
    //backpackModel.Draw(lightingShader);


    // render test cube
    ourCube.draw(lightingShader, glm::vec3(0.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    ourBillboard.draw(lightingShader, glm::vec3(1.0f, -0.15f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f));

    // render test plane
    ourPlane.draw(lightingShader, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(3.0f, 3.0f, 3.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    //ourCubes.Draw(lightingShader);


    ourSkybox.draw(projection, view);
}

void drawUI(Shader& lightingShader, float framerate)
{
    // render HUD / UI
    ourText.Draw(std::format("{} FPS", (int)framerate), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}

void cleanScene()
{
    ourSkybox.clean();

    ourCube.clean();
    ourPlane.clean();
    ourBillboard.clean();
}


