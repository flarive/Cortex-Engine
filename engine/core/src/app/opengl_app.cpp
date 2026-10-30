#include "../../include/app/opengl_app.h"

engine::OpenGLApp::OpenGLApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("OpenGLApp {} constructor called", _title);

    setup();
}

void engine::OpenGLApp::start()
{

}

void engine::OpenGLApp::setup()
{
    initGLFW();

    const char* glsl_version = initBackend();

    initWindow();

    setApp(window);

    // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    enableVerticalSync(true);

    enableMouseCapture(true);

    initGLAD();

    initImGUI(glsl_version);

    registerWindowInputCallbacks();

#if DEBUG
    // check opengl context is valid
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        // enable openGL debut ouput
        RenderState::enableDebugOutput();
        glDebugMessageCallback(openglDebugCallback, nullptr);
    }
    else {
        logger.warn("OpenGL debug context not available.");
    }
#endif
}

void engine::OpenGLApp::initGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        logger.error("GLFW init failed");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA
}

const char* engine::OpenGLApp::initBackend()
{
    #ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // enable OpenGL debug output
    #else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
    #endif

    // GL 3.3 + GLSL 130
    const char* glsl_version = "#version 130";

    // opengl 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only and macOS

    return glsl_version;
}

void engine::OpenGLApp::initImGUI(const char* backend_version)
{
    App::initImGUI(backend_version);

    // Setup OpenGL Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(backend_version);
}

void engine::OpenGLApp::enableVerticalSync(bool enable)
{
    // This enables V-Sync, capping the frame rate to the monitor's refresh rate (usually 60Hz or 144Hz).
    glfwSwapInterval(enable ? 1 : 0);
}

void engine::OpenGLApp::enableMouseCapture(bool enable)
{
    // tell GLFW to capture our mouse
    if (!enable)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GLAPIENTRY engine::OpenGLApp::openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    // Ignore informational messages
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)// || severity == GL_DEBUG_SEVERITY_LOW || severity == GL_DEBUG_SEVERITY_MEDIUM)
        return;

    OpenGLDebug::debugMessage(source, type, id, severity, length, message, userParam);
}

void engine::OpenGLApp::exit()
{
    /*logger.info("Exiting app {}", m_title);

    m_sceneManager.clean();*/

    // imGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //unregisterWindowInputCallbacks();

    glfwDestroyWindow(window);

    glfwTerminate();
}

engine::OpenGLApp::~OpenGLApp()
{
    logger.trace("OpenGLApp {} destructor called", m_title);
}